const asyncHandler = require("express-async-handler");
const Todo = require("../models/todoModel");

// @desc Get all todos
// @route GET /todo
const getAllTodos = asyncHandler(async (req, res) => {
  // 전체 할 일 보기
  const todos = await Todo.find();
  res.status(200).json(todos);
});

// @desc Create a todo
// @route POST /todo
const createTodo = asyncHandler(async (req, res) => {
  // 새 할 일 추가하기
  console.log(req.body);
  const { title, description, completed } = req.body;
  if (!title || !description || !completed) {
    return res.status(400).json({
      success: false,
      message: "필수값이 입력되지 않았습니다.",
    });
  }

  const todo = await Todo.create({
    title,
    description,
    completed,
  });

  console.log(todo);
  res.status(201).json(todo);
});

// @desc Get todo
// @route GET /todo/:id
const getTodo = asyncHandler(async (req, res) => {
  // 할 일 상세 보기
  const todo = await Todo.findById(req.params.id);
  if (!todo) {
    res.status(404);
    throw new Error("Todo not found");
  }
  res.status(200).json(todo);
});

// @desc Update todo
// @route PUT /todo/:id
const updateTodo = asyncHandler(async (req, res) => {
  // 할 일 수정하기
  const id = req.params.id;
  const { title, description, completed } = req.body;
  const todo = await Todo.findById(id);
  if (!todo) {
    res.status(404);
    throw new Error("Todo not found");
  }

  // 수정
  todo.title = title;
  todo.description = description;
  todo.completed = completed;

  // 저장
  const updatedTodo = await todo.save();
  res.status(200).json(updatedTodo);
});

// @desc Delete todo
// @route DELETE /todo/:id
const deleteTodo = asyncHandler(async (req, res) => {
  // 할 일 삭제하기
  const todo = await Todo.findById(req.params.id);
  if (!todo) {
    res.status(404);
    throw new Error("Todo not found");
  }
  await todo.deleteOne();
  res.status(200).json({ message: "할 일이 삭제되었습니다." });
});

module.exports = {
  getAllTodos,
  createTodo,
  getTodo,
  updateTodo,
  deleteTodo,
};
