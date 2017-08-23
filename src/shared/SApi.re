
module type Endpoint = {
  type request;
  type response;
  let path: string;
  let method: [`GET | `POST];
};

module type Get = {
  type response;
  let path: string;
};
module type Post = {
  type request;
  type response;
  let path: string;
};

let module Endpoints = {
  open Types;
  let module Todos = {
    type response = todos;
    let path = "/todos";
    let method = `GET;
  };
  let module AddTodo = {
    type request = string;
    type response = todos;
    let path = "/todo/add";
    let method = `POST;
  };
  let module RemoveTodo = {
    type request = int;
    type response = todos;
    let path = "/todo/remove";
    let method = `POST;
  };
  let module UpdateTodo = {
    type request = todo;
    type response = todos;
    let path = "/todo";
    let method = `POST;
  };
};


module type Config = {
  type t;
  module type Done;
  module Get: (Endpoint: Get) => Done;
  module Post: (Endpoint: Post) => Done;
};

let module Setup (Config: Config) => {
};
