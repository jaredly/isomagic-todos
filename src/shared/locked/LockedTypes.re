[@ocaml.warning "-34"];
module Types1 = {
  type _Types__id = int
  and _Types__text = string
  and _Types__todo =
    Types.todo = {
      text: _Types__text,
      id: _Types__id,
      completed: option(int),
    }
  and _Types__todos = list(_Types__todo);
};
let currentVersion = 1;
