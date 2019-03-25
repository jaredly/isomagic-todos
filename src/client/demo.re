Js.log("Hello, BuckleScript and Reason!");

type sum =
  | One(int)
  | Two(float, string)
  | Three;

type my_int = int;

type string_list = list(string);

type nested = {name: string};

type jumbo = {
  str: string,
  num: int,
  float,
  my_int,
  string_list,
  nums: array(int),
  nested,
  cross_module: Lib.person,
  sum: list(option(sum)),
};

Devtools.register();

let value = {
  str: "str",
  num: 10,
  float: 5.2,
  my_int: 3,
  string_list: ["one", "two", "threee"],
  nums: [|1, 2, 3|],
  nested: {
    name: "Me",
  },
  cross_module: {
    Lib.name: "Reason",
  },
  sum: [None, None, Some(Three), Some(One(2)), Some(Two(4.5, "hi"))],
};

Js.log(value);

Js.log(10);

let ss = value;

Js.log(ss);