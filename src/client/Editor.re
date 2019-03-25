let str = ReasonReact.string;

let style = ReactDOMRe.Style.make;

let evtValue = event =>
  ReactDOMRe.domElementToObj(ReactEventRe.Form.target(event))##value;

type retainedProps = {value: string};

let component = ReasonReact.reducerComponentWithRetainedProps("Editor");

type state = option(string);

let make = (~value, ~onChange, ~placeholder, ~className="", ~clear=false, _) => {
  ...component,
  initialState: () => None,
  retainedProps: {
    value: value,
  },
  willReceiveProps: ({state, retainedProps}) =>
    switch (state) {
    | None => None
    | Some(text) =>
      retainedProps.value === value && value !== text ? Some(text) : None
    },
  reducer: (action, state) => ReasonReact.Update(action),
  render: ({state, send}) =>
    switch (state) {
    | None =>
      <div
        style={style(~flex="1", ~padding="3px", ~cursor="text", ())}
        className
        onClick={evt => {
          ReactEventRe.Mouse.stopPropagation(evt);
          send(Some(value));
        }}>
        {str(value === "" ? placeholder : value)}
      </div>
    | Some(text) =>
      <input
        value=text
        placeholder
        className
        autoFocus=true
        onChange={evt => send(Some(evtValue(evt)))}
        onClick={evt => ReactEventRe.Mouse.stopPropagation(evt)}
        onKeyDown={evt =>
          switch (ReactEventRe.Keyboard.key(evt)) {
          | "Enter" =>
            if (text == value) {
              send(None);
            } else {
              onChange(text);
              if (clear) {
                send(Some(value));
              };
            }
          | _ => ()
          }
        }
        style={style(
          ~fontFamily="inherit",
          ~flex="1",
          ~fontSize="inherit",
          (),
        )}
        onBlur={_ =>
          if (text != value) {
            onChange(text); /* TODO I want a different state value that is "waiting" */
            if (clear) {
              send(Some(value));
            };
          } else {
            send(None);
          }
        }
      />
    },
};