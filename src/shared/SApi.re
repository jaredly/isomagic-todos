module type Endpoint = {
  type request;
  type response;
  let path: string;
  let method: [ | `GET | `POST];
};

module type SerdeType = {
  type t;
  let serialize: t => Serde.target;
  let deserialize: Serde.target => Belt.Result.t(t, list(string));
};

// TODO serializeResponse, deserializeResponse or something...
module type Get = {
  module Response: SerdeType;
  let path: string;
};

module type Post = {
  module Request: SerdeType;
  module Response: SerdeType;
  let path: string;
};

// module type Serde = {
//   type t;
//   let serializeTodo: Types.todo => t;
//   let deserializeTodo: t => Belt.Result.t(Types.todo, list(string));
//   let serializeTodos: Types.todos => t;
//   let deserializeTodos: t => Belt.Result.t(Types.todos, list(string));
// };

module Endpoints = {
  open Types;
  // open! Serde.Modules;
  module Todos = {
    module Response = Serde.Modules.Todos;
    let path = "/todos";
    let pub_ = `GET;
  };
  module AddTodo = {
    module Request = Serde.Modules.Text;
    module Response = Serde.Modules.Todos;
    type request = string;
    type response = todos;
    let path = "/todo/add";
    let pub_ = `POST;
  };
  module RemoveTodo = {
    module Request = Serde.Modules.Id;
    module Response = Serde.Modules.Todos;
    let path = "/todo/remove";
    let pub_ = `POST;
  };
  module UpdateTodo = {
    module Request = Serde.Modules.Todo;
    module Response = Serde.Modules.Todos;
    let path = "/todo";
    let pub_ = `POST;
  };
};

module type Config = {
  type t;
  module type Done;
  module Get: (Endpoint: Get) => Done;
  module Post: (Endpoint: Post) => Done;
};

module Setup = (Config: Config) => {};