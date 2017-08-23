
let str = ReasonReact.stringToElement;
let style = ReactDOMRe.Style.make;
let evtValue event => (ReactDOMRe.domElementToObj (ReactEventRe.Form.target event))##value;

type retainedProps = {value: string};
let component = ReasonReact.statefulComponentWithRetainedProps "Editor";
type state = option string;
let make ::value ::onChange ::placeholder ::className="" ::clear=false _ => {
  ...component,
  initialState: fun () => None,
  retainedProps: {value: value},
  willReceiveProps: fun {state, retainedProps} => switch state {
  | None => None
  | Some text => (retainedProps.value === value && value !== text) ? Some text : None
  },
  render: fun {state, update} => {
    switch state {
    | None => <div
        style=(style cursor::"text" ())
        className
        onClick=(fun evt => {
          ReactEventRe.Mouse.stopPropagation evt;
          (update (fun _ _ => ReasonReact.Update (Some value))) evt;
          })
      >(str (value === "" ? placeholder : value))</div>
    | Some text => <input
        value=text
        placeholder
        className
        autoFocus=(Js.Boolean.to_js_boolean true)
        onChange=(update (fun evt _ => ReasonReact.Update (Some (evtValue evt))))
        onClick=(fun evt => ReactEventRe.Mouse.stopPropagation evt)
        onKeyDown=(fun evt => {
          switch (ReactEventRe.Keyboard.key evt) {
          | "Enter" => if (text == value) {
            (update (fun _ _ => ReasonReact.Update None)) ()
          } else {
            onChange text;
            if (clear) {
              (update (fun _ _ => ReasonReact.Update (Some value))) ()
            }
          }
          | _ => ()
          }
        })
        style=(style fontFamily::"inherit" flex::"1" fontSize::"inherit" ())
        onBlur=(fun _ => {
          if (text != value) {
            onChange text; /* TODO I want a different state value that is "waiting" */
            if (clear) {
              (update (fun _ _ => ReasonReact.Update (Some value))) ()
            }
          } else {
            (update (fun _ _ => ReasonReact.Update None)) ()
          }
        })
      />
    }
  }
};
