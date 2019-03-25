open All_types.Types;

type appState = {
  nextId: int,
  todos
};

let appState = ref({nextId: 1, todos: [{id: 0, text: "Add some things to do", completed: None}]});

Server.get_prefix("/", Server.serveStatic("../public"));

module type E = {};

let _: list((module E)) = [
  (module
   Server.Get(
     SApi.Endpoints.Todos,
     {
       let handle = (req) => appState^.todos;
     }
   )),
  (module
   Server.Post(
     SApi.Endpoints.AddTodo,
     {
       let handle = (req, text) => {
         let id = appState^.nextId;
         let todo = {completed: None, id, text};
         let todos = List.append(appState^.todos, [todo]);
         appState := {nextId: id + 1, todos};
         todos;
       };
     }
   )),
  (module
   Server.Post(
     SApi.Endpoints.RemoveTodo,
     {
       let handle = (req, id) => {
         let todos = List.filter((item) => item.id !== id, appState^.todos);
         appState := {...appState^, todos};
         todos;
       };
     }
   )),
  (module
   Server.Post(
     SApi.Endpoints.UpdateTodo,
     {
       let handle = (req, data) => {
         let todos = List.map((item) => item.id === data.id ? data : item, appState^.todos);
         appState := {...appState^, todos};
         todos;
       };
     }
   ))
]; /* fallback */

Server.get_prefix(
  "/",
  (_, _, _, _) =>
    Cohttp_lwt_unix.Server.respond_string(~status=`Not_found, ~body="Welcome to never land", ())
);

Server.listen(8000);
