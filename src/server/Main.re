let module CoServer = Cohttp_lwt_unix.Server;
open Shared.Types;

type appState = {
  nextId: int,
  todos: todos,
};

let appState = ref {
  nextId: 1,
  todos: [{
    id: 0,
    text: "Add some things to do",
    completed: None,
  }]
};

let maybe_parse text => try(Some (Yojson.Safe.from_string text)) {
| _ => None
};

let withBody body fn => Lwt.(Cohttp_lwt_body.to_string body >>= fn);
let withJsonBody body fn => withBody body (fun body => fn (maybe_parse body));
let withParsedBody body parse fn => withBody body (fun body => switch (maybe_parse body) {
  | None => CoServer.respond_string status::`Bad_request body::"" ()
  | Some json => switch (parse json) {
    | None => CoServer.respond_string status::`Bad_request body::"" ()
    | Some data => fn data
  }
});

Server.get_prefix "/" (Server.serveStatic "../public");

Server.get "/todos" (fun _ _ _ => Server.json (todos__to_yojson (!appState).todos));

Server.post "/todo/add" (fun _ body _ => withParsedBody body new_todo__from_yojson (fun text => {
  let id = (!appState).nextId;
  let todo = {completed: None, id, text};
  let todos = List.append (!appState).todos [todo];
  appState := {
    nextId: id + 1,
    todos,
  };
  Server.json (todos__to_yojson todos)
}));

Server.post "/todo/remove" (fun _ body _ => withParsedBody body int__from_yojson (fun id => {
  let todos = List.filter (fun item => item.id !== id) (!appState).todos;
  appState := {...!appState, todos};
  Server.json (todos__to_yojson todos)
}));

let module Let_syntax = {
  let bind value ::f => Lwt.bind value f;
  let map value ::f => Lwt.map f value;
};

Server.post "/todo" Lwt.(fun _ body _ => {
  [%await let body = Cohttp_lwt_body.to_string body];

  [%guard let Some(data) = maybe_parse body]
  [@else CoServer.respond_string status::`Bad_request body::"" ()];

  [%guard let Some(data) = todo__from_yojson data]
  [@else CoServer.respond_string status::`Bad_request body::"" ()];

  let m = ref 2;
  m := 3;
  let todos = List.map (fun item => item.id === data.id ? data : item) (!appState).todos;
  appState := {
    ...!appState,
    todos,
  };
  Server.json (todos__to_yojson todos);
});

/* fallback */
Server.get_prefix "/" (fun _ _ _ _ => {
  CoServer.respond_string status::`Not_found body::"Welcome to never land" ()
});

Server.listen 8000;
