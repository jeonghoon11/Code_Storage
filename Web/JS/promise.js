function add10(num) {
  const promise = new Promise((resolve, reject) => {
    setTimeout(() => {
      if (typeof num === "number") {
        resolve(num + 10);
      } else reject("숫자가 아닙니다.");
    }, 2000);
  });
  return promise;
}

add10(0)
  .then((value) => {
    console.log(value);
    return add10(value);
  })
  .then((value) => {
    console.log(value);
    return add10(undefined);
  })
  .then((value) => {
    console.log(value);
    return add10(value);
  })
  .catch((error) => {
    console.log(error);
  });
