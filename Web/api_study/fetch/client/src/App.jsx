// fetch
// axios

import { useEffect, useState } from "react";
import axios from "axios";
import "./App.css";

const SERVER_URL = "http://localhost:4000/api/todo";

function App() {
  const [todoList, setTodoList] = useState([]);

  const fetchData = async () => {
    const response = await axios.get("SERVER_URL");
    setTodoList(response.data);
  };
  fetch("SERVER_URL")
    .then((response) => response.json())
    .then((data) => setTodoList(data));
}

//useEffect를 사용하지 않으면 React 특성상 값을 받아올 때 계속 리렌더링되게 된다.
//따라서 처음 마운트 되었을 때 작동하도록 deps에 빈 배열을 넣어준다.
useEffect(() => {
  fetchData();
}, []);

const onSubmitHandler = async (e) => {
  e.preventDefault();
  const text = e.target.text.value;
  const done = e.target.done.checked;
  axios.post("SERVER_URL", { text, done }); // axios는 headers, body의 내용을 이처럼 처리해준다.
  fetchData();
  //   fetch("http://localhost:4000/api/todo", {
  //     method: "POST",
  //     headers: {
  //       //데이터를 입력해서 보낼 대 headers를 추가한다.
  //       "Content-Type": "application/json",
  //     },
  //     body: JSON.stringify({
  //       text,
  //       done,
  //     }), //이렇게만 하면 렌더링이 되지 않는다. 따라서 then메서드를 추가해야된다.
  //   }).then(() => fetchData());
  // };

  return (
    <div className="App">
      <h1>Todo List</h1>
      <form onSubmit={onSubmitHandler}>
        <input name="text" />
        <input name="done" type="checkbox" />
        <input type="submit" value="추가" />
      </form>

      {todoList.map((todo) => (
        <div key={todo.id} style={{ display: "flex" }}>
          <div>{todo.id}</div>
          <div>{todo.text}</div>
          <div>{todo.done ? "Y" : "N"}</div>
        </div>
      ))}
    </div>
  );
};

export default App;
