
type id = int;
type text = string;

type person = {
  id,
  name: string,
  age: int,
  friends: list(int)
};

type todo = {
  text,
  id,
  completed: option(int)
};

type todos = list(todo);

type new_todo = string;
