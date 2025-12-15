require("dotenv").config();

const mongoose = require("mongoose");
const dbConnect = async () => {
  try {
    const connect = await mongoose.connect(process.env.DB_CONNECT);
    console.log("Mongo DB connected");
  } catch (err) {
    console.log(err);
  }
};

module.exports = dbConnect;
