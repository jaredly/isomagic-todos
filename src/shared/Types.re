
type person = {
  id: int,
  name: string,
  age: int,
  friends: list int,
};

type todo = {
  text: string,
  id: int,
  completed: option int,
};

type todos = list todo;

type new_todo = string;