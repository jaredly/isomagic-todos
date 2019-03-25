open! Shared.Types;

let post: (string, Js.Json.t, Js.Json.t => unit) => unit = [%bs.raw
  {|
  function (path, json, onDone) {
    fetch(path, {body: JSON.stringify(json), method: 'POST', headers: {'Content-Type': 'application/json'}})
    .then(res => res.json())
    .then(res => onDone(res), err => console.error('failed', err))
  }
|}
];

module Post = (Config: SApi.Post) => {
  let run = (body, onDone) =>
    post(
      Config.path,
      Config.Request.serialize(body),
      (data) =>
        switch (Config.Response.deserialize(data)) {
        | Error(error) => Js.log("Failed to parse data")
        | Ok(result) =>
          // Js.log(Config.Response.(result));
          onDone(result);
        }
    );
};

module RemoveTodo = Post(SApi.Endpoints.RemoveTodo);

module AddTodo = Post(SApi.Endpoints.AddTodo);

module UpdateTodo = Post(SApi.Endpoints.UpdateTodo);
