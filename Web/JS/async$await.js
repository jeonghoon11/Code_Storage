//async : 어떤 함수를 비동기 함수로 만들어준다. 이때 함수가 Promise를 반환하도록 해준다.
// async function getData() {
//   return {
//     name: "장정훈",
//     id: "jeonghoon",
//   };
// }

// console.log(getData());

//async가 반환하는게 promise객체이면 그냥 Promise객체를 반환한다.
async function getData() {
  return new Promise((resolve, reject) => {
    setTimeout(() => {
      resolve({
        name: "장정훈",
        id: "jeonghoon",
      });
    }, 1500);
  });
}

// function printData() {
//   getData().then((result) => {
//     console.log(result);
//   });
// }

// await : async 함수 내부에서만 사용이 가능한 키워드.
// 비동기 함수가 다 처리되기를 기다리는 역할
// resolve, then을 사용하지 않아도 된다.
async function printData() {
  const data = await getData();
  console.log(data);
}

printData();
