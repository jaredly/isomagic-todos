[@ocaml.warning "-34"];
open LockedTypes;
type target = Js.Json.t;
module Version1 = {
  open Types1;
  let rec deserialize_Types____id:
    Js.Json.t => Belt.Result.t(_Types__id, list(string)) =
    value =>
      (
        number =>
          switch (Js.Json.classify(number)) {
          | JSONNumber(number) => Belt.Result.Ok(int_of_float(number))
          | _ => Error(["Expected a float"])
          }
      )(
        value,
      )
  and deserialize_Types____text:
    Js.Json.t => Belt.Result.t(_Types__text, list(string)) =
    value =>
      (
        string =>
          switch (Js.Json.classify(string)) {
          | JSONString(string) => Belt.Result.Ok(string)
          | _ => Error(["expected a string"])
          }
      )(
        value,
      )
  and deserialize_Types____todo:
    Js.Json.t => Belt.Result.t(_Types__todo, list(string)) =
    record =>
      switch (Js.Json.classify(record)) {
      | JSONObject(dict) =>
        let inner = attr_completed => {
          let inner = attr_id => {
            let inner = attr_text =>
              Belt.Result.Ok(
                {text: attr_text, id: attr_id, completed: attr_completed}: _Types__todo,
              );
            switch (Js.Dict.get(dict, "text")) {
            | None => Belt.Result.Error(["No attribute 'text'"])
            | Some(json) =>
              switch (deserialize_Types____text(json)) {
              | Belt.Result.Error(error) =>
                Belt.Result.Error(["attribute 'text'", ...error])
              | Ok(data) => inner(data)
              }
            };
          };
          switch (Js.Dict.get(dict, "id")) {
          | None => Belt.Result.Error(["No attribute 'id'"])
          | Some(json) =>
            switch (deserialize_Types____id(json)) {
            | Belt.Result.Error(error) =>
              Belt.Result.Error(["attribute 'id'", ...error])
            | Ok(data) => inner(data)
            }
          };
        };
        switch (Js.Dict.get(dict, "completed")) {
        | None => inner(None)
        | Some(json) =>
          switch (
            (
              (
                (transformer, option) =>
                  switch (Js.Json.classify(option)) {
                  | JSONNull => Belt.Result.Ok(None)
                  | _ =>
                    switch (transformer(option)) {
                    | Belt.Result.Error(error) =>
                      Belt.Result.Error(["optional value", ...error])
                    | Ok(value) => Ok(Some(value))
                    }
                  }
              )(
                number =>
                switch (Js.Json.classify(number)) {
                | JSONNumber(number) => Belt.Result.Ok(int_of_float(number))
                | _ => Error(["Expected a float"])
                }
              )
            )(
              json,
            )
          ) {
          | Belt.Result.Error(error) =>
            Belt.Result.Error(["attribute 'completed'", ...error])
          | Ok(data) => inner(data)
          }
        };
      | _ => Belt.Result.Error(["Expected an object"])
      }
  and deserialize_Types____todos:
    Js.Json.t => Belt.Result.t(_Types__todos, list(string)) =
    value =>
      (
        list =>
          switch (Js.Json.classify(list)) {
          | JSONArray(items) =>
            let transformer = deserialize_Types____todo;
            let rec loop = (i, collected, items) =>
              switch (items) {
              | [] => Belt.Result.Ok(Belt.List.reverse(collected))
              | [one, ...rest] =>
                switch (transformer(one)) {
                | Belt.Result.Error(error) =>
                  Belt.Result.Error([
                    "list element " ++ string_of_int(i),
                    ...error,
                  ])
                | Belt.Result.Ok(value) =>
                  loop(i + 1, [value, ...collected], rest)
                }
              };
            loop(0, [], Belt.List.fromArray(items));
          | _ => Belt.Result.Error(["expected an array"])
          }
      )(
        value,
      )
  and serialize_Types____id: _Types__id => Js.Json.t =
    value => (int => Js.Json.number(float_of_int(int)))(value)
  and serialize_Types____text: _Types__text => Js.Json.t =
    value => Js.Json.string(value)
  and serialize_Types____todo: _Types__todo => Js.Json.t =
    record =>
      Js.Json.object_(
        Js.Dict.fromArray([|
          ("text", serialize_Types____text(record.text)),
          ("id", serialize_Types____id(record.id)),
          (
            "completed",
            (
              (
                transformer =>
                  fun
                  | Some(inner) => transformer(inner)
                  | None => Js.Json.null
              )(
                int =>
                Js.Json.number(float_of_int(int))
              )
            )(
              record.completed,
            ),
          ),
        |]),
      )
  and serialize_Types____todos: _Types__todos => Js.Json.t =
    value =>
      (
        list =>
          Js.Json.array(
            Belt.List.toArray(Belt.List.map(list, serialize_Types____todo)),
          )
      )(
        value,
      );
};
module Current = Version1;
let parseVersion = json =>
  switch (Js.Json.classify(json)) {
  | JSONObject(dict) =>
    switch (Js.Dict.get(dict, "$schemaVersion")) {
    | Some(schemaVersion) =>
      switch (Js.Json.classify(schemaVersion)) {
      | JSONNumber(version) =>
        [@implicit_arity] Belt.Result.Ok(int_of_float(version), json)
      | _ => Belt.Result.Error("Invalid $schemaVersion")
      }
    | None => Belt.Result.Error("No $schemaVersion present")
    }
  | JSONArray([|version, payload|]) =>
    switch (Js.Json.classify(version)) {
    | JSONNumber(version) =>
      [@implicit_arity] Belt.Result.Ok(int_of_float(version), payload)
    | _ => Belt.Result.Error("Invalid wrapped version")
    }
  | _ => Belt.Result.Error("Must have a schema version")
  };
let wrapWithVersion = (version, payload) =>
  switch (Js.Json.classify(payload)) {
  | JSONObject(dict) =>
    Js.Dict.set(
      dict,
      "$schemaVersion",
      Js.Json.number(float_of_int(version)),
    );
    Js.Json.object_(dict);
  | _ => Js.Json.array([|Js.Json.number(float_of_int(version)), payload|])
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
