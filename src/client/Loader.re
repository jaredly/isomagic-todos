
module type Config = {
  type data;
  type state;
  let url: string;
  let data__from_json: Js.Json.t => option data;
  let data__to_json: data => Js.Json.t;
  let data__to_devtools: data => Js.t {.};
  let make: data::data => array int =>  ReasonReact.componentSpec state state ReasonReact.noRetainedProps ReasonReact.noRetainedProps;
};

let str = ReasonReact.stringToElement;
let style = ReactDOMRe.Style.make;

let fetchJson: string => (Js.Json.t => unit) => unit = [%bs.raw {| function (url, fn) {
  fetch(url).then(res => res.json()).then(fn)
} |}];

let module F (Config: Config) => {
  type state = Loading | Error string | Loaded Config.data;
  let component = ReasonReact.statefulComponent "Loader";
  let make _children => {
    ...component,
    initialState: fun () => Loading,
    didMount: fun {update} => {
      fetchJson Config.url (update (fun json _ => switch (Config.data__from_json json) {
      | None => ReasonReact.NoUpdate
      | Some data => ReasonReact.Update (Loaded data)
      }));
      ReasonReact.NoUpdate
    },
    render: fun self => {
      switch self.state {
      | Loading => <div style=(style alignItems::"center" padding::"5px" color::"#ddd" ())>(str "Loading")</div>
      | Error text => <div>(str "Error") (str text)</div>
      | Loaded data => <Config data />
      }
    }
  };
};
