
open! Shared.Types;

let post: string => Js.Json.t => (Js.Json.t => unit) => unit = [%bs.raw {|
  function (path, json, onDone) {
    fetch(path, {body: JSON.stringify(json), method: 'POST', headers: {'Content-Type': 'application/json'}})
    .then(res => res.json())
    .then(res => onDone(res), err => console.error('failed', err))
  }
|}];

let module Post (Config: SApi.Post) => {
  let run body onDone => post Config.path (Config.request__to_json body) (fun data => {
    switch (Config.response__from_json data) {
    | None => Js.log "Failed to parse data"
    | Some result => {
      Js.log (Config.response__to_devtools result);
      onDone result
    }
    }
  })
};

let module RemoveTodo = Post (SApi.Endpoints.RemoveTodo);
let module AddTodo = Post (SApi.Endpoints.AddTodo);
let module UpdateTodo = Post (SApi.Endpoints.UpdateTodo);
