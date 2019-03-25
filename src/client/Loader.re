module type Config = {
  module Data: SApi.SerdeType;
  type state;
  type action;
  let url: string;
  // let data__from_json: Js.Json.t => option(data);
  // let data__to_json: data => Js.Json.t;
  // let data__to_devtools: data => Js.t({.});
  let make:
    (~data: Data.t, array(int)) =>
    ReasonReact.componentSpec(
      state,
      state,
      ReasonReact.noRetainedProps,
      ReasonReact.noRetainedProps,
      action,
    );
};

let str = ReasonReact.string;

let style = ReactDOMRe.Style.make;

let fetchJson: (string, Js.Json.t => unit) => unit = [%bs.raw
  {| function (url, fn) {
  fetch(url).then(res => res.json()).then(fn)
} |}
];

module F = (Config: Config) => {
  type state =
    | Loading
    | Error(string)
    | Loaded(Config.Data.t);
  let component = ReasonReact.reducerComponent("Loader");
  let make = _children => {
    ...component,
    initialState: () => Loading,
    reducer: (action, state) => ReasonReact.Update(action),
    didMount: ({send}) => {
      fetchJson(Config.url, json =>
        switch (Config.Data.deserialize(json)) {
        | Error(_error) => Js.log2("Failed to load", _error)
        | Ok(data) => send(Loaded(data))
        }
      );
    },
    render: self =>
      switch (self.state) {
      | Loading =>
        <div
          style={style(
            ~alignItems="center",
            ~padding="5px",
            ~color="#ddd",
            (),
          )}>
          {str("Loading")}
        </div>
      | Error(text) => <div> {str("Error")} {str(text)} </div>
      | Loaded(data) => <Config data />
      },
  };
};