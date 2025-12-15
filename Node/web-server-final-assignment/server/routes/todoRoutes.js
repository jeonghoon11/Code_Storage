const express = require("express");
const router = express.Router();

const {
  getAllTodos,
  createTodo,
  getTodo,
  updateTodo,
  deleteTodo,
} = require("../controllers/todoController4mongo");

router.route("/").get(getAllTodos).post(createTodo);
router.route("/:id").get(getTodo).put(updateTodo).delete(deleteTodo);

module.exports = router;
