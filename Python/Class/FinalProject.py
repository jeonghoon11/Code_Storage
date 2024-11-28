from cryptography.hazmat.primitives.asymmetric import dh
from cryptography.hazmat.primitives.kdf.hkdf import HKDF
from cryptography.hazmat.primitives.hashes import SHA256
from cryptography.hazmat.primitives.ciphers.aead import AESGCM
import os

# AES-GCM 암호화/복호화 함수
def encrypt_message(session_key, plaintext):
    aesgcm = AESGCM(session_key)
    nonce = os.urandom(12)
    ciphertext = aesgcm.encrypt(nonce, plaintext.encode(), None)
    return nonce, ciphertext

def decrypt_message(session_key, nonce, ciphertext):
    aesgcm = AESGCM(session_key)
    return aesgcm.decrypt(nonce, ciphertext, None).decode()

# Diffie-Hellman 키 교환 및 세션 키 생성
def diffie_hellman_key_exchange(parameters_a, parameters_b=None):
    private_key_a = parameters_a.generate_private_key()
    public_key_a = private_key_a.public_key()

    if parameters_b is None:
        parameters_b = parameters_a  # 기본적으로 동일한 파라미터 사용

    private_key_b = parameters_b.generate_private_key()
    public_key_b = private_key_b.public_key()

    try:
        shared_key_a = private_key_a.exchange(public_key_b)
        shared_key_b = private_key_b.exchange(public_key_a)

        common_salt = os.urandom(16)

        session_key_a = HKDF(
            algorithm=SHA256(),
            length=32,
            salt=common_salt,
            info=b'OTR Session Key'
        ).derive(shared_key_a)

        session_key_b = HKDF(
            algorithm=SHA256(),
            length=32,
            salt=common_salt,
            info=b'OTR Session Key'
        ).derive(shared_key_b)

        return session_key_a, session_key_b
    except ValueError as e:
        # 파라미터 불일치로 인해 공유키 계산 실패 시, None 반환
        print(f"키 교환 실패")
        return None, None

암호화 함수
def key_exchange(parameters_b=None):
    parameters_a = dh.generate_parameters(generator=2, key_size=2048)

    # 키 교환 수행
    session_key_a, session_key_b = diffie_hellman_key_exchange(parameters_a, parameters_b)

    if session_key_a is None or session_key_b is None:
        print(f"키 일치 여부: False")
        print(f"사용자 A의 세션 키: None")
        print(f"사용자 B의 세션 키: None")
        return

    # 암호화/복호화
    original_message = "이 메시지는 OTR 암호화를 사용하여 암호화되었습니다."
    nonce, ciphertext = encrypt_message(session_key_a, original_message)
    decrypted_message = decrypt_message(session_key_b, nonce, ciphertext)

    if session_key_a == session_key_b:
        print(f"키 일치 여부: True")
        print(f"사용자 A의 세션 키: {session_key_a}")
        print(f"사용자 B의 세션 키: {session_key_b}")
        print(f"원본 메시지: {original_message}")
        print(f"암호화된 메시지: {ciphertext}")
        print(f"복호화된 메시지: {decrypted_message}")

        if original_message == decrypted_message:
            print("복호화 성공: 원본 메시지와 일치합니다.")
        else:
            print("복호화 실패: 원본 메시지와 일치하지 않습니다.")
    else:
        print(f"키 일치 여부: False")
        print(f"사용자 A의 세션 키: {session_key_a}")
        print(f"사용자 B의 세션 키: {session_key_b}")

# 암호화 성공
# key_exchange()

# 암호화 실패
key_exchange(dh.generate_parameters(generator=5, key_size=2048))