const express = require("express");
const app = express();
const dbConnect = require("./config/dbConnect4mongo");

const port = 3000;
dbConnect();

// 미들웨어 설정
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// 라우트 설정
app.use("/todos", require("./routes/todoRoutes"));

app.listen(port, () => {
  console.log(`${port}번 포트에서 서버 실행 중`);
});
