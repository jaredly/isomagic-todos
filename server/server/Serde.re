[@ocaml.warning "-34"];
open LockedTypes;
type target = Ezjsonm.value;
module Version1 = {
  open Types1;
  let rec deserialize_Types____id:
    Ezjsonm.value => Belt.Result.t(_Types__id, list(string)) =
    value =>
      (
        number =>
          switch (number) {
          | `Float(number) => Belt.Result.Ok(int_of_float(number))
          | _ => Error(["Expected a float"])
          }
      )(
        value,
      )
  and deserialize_Types____text:
    Ezjsonm.value => Belt.Result.t(_Types__text, list(string)) =
    value =>
      (
        string =>
          switch (string) {
          | `String(string) => Belt.Result.Ok(string)
          | _ => Error(["epected a string"])
          }
      )(
        value,
      )
  and deserialize_Types____todo:
    Ezjsonm.value => Belt.Result.t(_Types__todo, list(string)) =
    record =>
      switch (record) {
      | `O(items) =>
        let inner = attr_completed => {
          let inner = attr_id => {
            let inner = attr_text =>
              Belt.Result.Ok(
                {text: attr_text, id: attr_id, completed: attr_completed}: _Types__todo,
              );
            switch (Belt.List.getAssoc(items, "text", (==))) {
            | None => Belt.Result.Error(["No attribute 'text'"])
            | Some(json) =>
              switch (deserialize_Types____text(json)) {
              | Belt.Result.Error(error) =>
                Belt.Result.Error(["attribute 'text'", ...error])
              | Belt.Result.Ok(data) => inner(data)
              }
            };
          };
          switch (Belt.List.getAssoc(items, "id", (==))) {
          | None => Belt.Result.Error(["No attribute 'id'"])
          | Some(json) =>
            switch (deserialize_Types____id(json)) {
            | Belt.Result.Error(error) =>
              Belt.Result.Error(["attribute 'id'", ...error])
            | Belt.Result.Ok(data) => inner(data)
            }
          };
        };
        switch (Belt.List.getAssoc(items, "completed", (==))) {
        | None => inner(None)
        | Some(json) =>
          switch (
            (
              (
                (transformer, option) =>
                  switch (option) {
                  | `Null => Belt.Result.Ok(None)
                  | _ =>
                    switch (transformer(option)) {
                    | Belt.Result.Error(error) =>
                      Belt.Result.Error(["optional value", ...error])
                    | Belt.Result.Ok(value) => Belt.Result.Ok(Some(value))
                    }
                  }
              )(
                number =>
                switch (number) {
                | `Float(number) => Belt.Result.Ok(int_of_float(number))
                | _ => Error(["Expected a float"])
                }
              )
            )(
              json,
            )
          ) {
          | Belt.Result.Error(error) =>
            Belt.Result.Error(["attribute 'completed'", ...error])
          | Belt.Result.Ok(data) => inner(data)
          }
        };
      | _ => Belt.Result.Error(["Expected an object"])
      }
  and deserialize_Types____todos:
    Ezjsonm.value => Belt.Result.t(_Types__todos, list(string)) =
    value =>
      (
        list =>
          switch (list) {
          | `A(items) =>
            let transformer = deserialize_Types____todo;
            let rec loop = (collected, items) =>
              switch (items) {
              | [] => Belt.Result.Ok(Belt.List.reverse(collected))
              | [one, ...rest] =>
                switch (transformer(one)) {
                | Belt.Result.Error(error) =>
                  Belt.Result.Error(["list item", ...error])
                | Belt.Result.Ok(value) => loop([value, ...collected], rest)
                }
              };
            loop([], items);
          | _ => Belt.Result.Error(["expected an array"])
          }
      )(
        value,
      )
  and serialize_Types____id: _Types__id => Ezjsonm.value =
    value => (i => `Float(float_of_int(i)))(value)
  and serialize_Types____text: _Types__text => Ezjsonm.value =
    value => (s => `String(s))(value)
  and serialize_Types____todo: _Types__todo => Ezjsonm.value =
    record =>
      `O([
        ("text", serialize_Types____text(record.text)),
        ("id", serialize_Types____id(record.id)),
        (
          "completed",
          (
            (
              transformer =>
                fun
                | None => `Null
                | Some(v) => transformer(v)
            )(
              i =>
              `Float(float_of_int(i))
            )
          )(
            record.completed,
          ),
        ),
      ])
  and serialize_Types____todos: _Types__todos => Ezjsonm.value =
    value =>
      (list => `A(Belt.List.map(list, serialize_Types____todo)))(value);
};
module Current = Version1;
let parseVersion = json =>
  switch (json) {
  | `O(items) =>
    switch ((Belt.List.getAssoc(items))("$schemaVersion", (==))) {
    | Some(`Float(schemaVersion)) =>
      [@implicit_arity] Belt.Result.Ok(int_of_float(schemaVersion), json)
    | Some(_) => Belt.Result.Error("Invalid schema version - expected number")
    | None => Belt.Result.Error("No $schemaVersion")
    }
  | `A([`Float(version), payload]) =>
    [@implicit_arity] Belt.Result.Ok(int_of_float(version), payload)
  | _ => Belt.Result.Error("Not wrapped in a version")
  };
let wrapWithVersion = (version, payload) =>
  switch (payload) {
  | `O(items) =>
    `O([("$schemaVersion", `Float(float_of_int(version))), ...items])
  | _ => `A([`Float(float_of_int(version)), payload])
  };
let serializeTodo = data =>
  wrapWithVersion(currentVersion, Version1.serialize_Types____todo(data))
and deserializeTodo = data =>
  switch (parseVersion(data)) {
  | Belt.Result.Error(err) => Belt.Result.Error([err])
  | [@implicit_arity] Ok(version, data) =>
    switch (version) {
    | 1 =>
      switch (Version1.deserialize_Types____todo(data)) {
      | Belt.Result.Error(error) => Belt.Result.Error(error)
      | Ok(data) => Belt.Result.Ok(data)
      }
    | _ =>
      Belt.Result.Error(["Unexpected version " ++ string_of_int(version)])
    }
  };
let serializeTodos = data =>
  wrapWithVersion(currentVersion, Version1.serialize_Types____todos(data))
and deserializeTodos = data =>
  switch (parseVersion(data)) {
  | Belt.Result.Error(err) => Belt.Result.Error([err])
  | [@implicit_arity] Ok(version, data) =>
    switch (version) {
    | 1 =>
      switch (Version1.deserialize_Types____todos(data)) {
      | Belt.Result.Error(error) => Belt.Result.Error(error)
      | Ok(data) => Belt.Result.Ok(data)
      }
    | _ =>
      Belt.Result.Error(["Unexpected version " ++ string_of_int(version)])
    }
  };
let serializeId = data =>
  wrapWithVersion(currentVersion, Version1.serialize_Types____id(data))
and deserializeId = data =>
  switch (parseVersion(data)) {
  | Belt.Result.Error(err) => Belt.Result.Error([err])
  | [@implicit_arity] Ok(version, data) =>
    switch (version) {
    | 1 =>
      switch (Version1.deserialize_Types____id(data)) {
      | Belt.Result.Error(error) => Belt.Result.Error(error)
      | Ok(data) => Belt.Result.Ok(data)
      }
    | _ =>
      Belt.Result.Error(["Unexpected version " ++ string_of_int(version)])
    }
  };
let serializeText = data =>
  wrapWithVersion(currentVersion, Version1.serialize_Types____text(data))
and deserializeText = data =>
  switch (parseVersion(data)) {
  | Belt.Result.Error(err) => Belt.Result.Error([err])
  | [@implicit_arity] Ok(version, data) =>
    switch (version) {
    | 1 =>
      switch (Version1.deserialize_Types____text(data)) {
      | Belt.Result.Error(error) => Belt.Result.Error(error)
      | Ok(data) => Belt.Result.Ok(data)
      }
    | _ =>
      Belt.Result.Error(["Unexpected version " ++ string_of_int(version)])
    }
  };
module Modules = {
  module Todo = {
    type t = Types1._Types__todo;
    let serialize = serializeTodo;
    let deserialize = deserializeTodo;
  };
  module Todos = {
    type t = Types1._Types__todos;
    let serialize = serializeTodos;
    let deserialize = deserializeTodos;
  };
  module Id = {
    type t = Types1._Types__id;
    let serialize = serializeId;
    let deserialize = deserializeId;
  };
  module Text = {
    type t = Types1._Types__text;
    let serialize = serializeText;
    let deserialize = deserializeText;
  };
};
