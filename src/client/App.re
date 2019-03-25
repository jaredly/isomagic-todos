open! Types;

let str = ReasonReact.string;

// let style = ReactDOMRe.Style.make;
let todoItemStyle = Css.(
          style([
            backgroundColor(white),
            alignItems(`center),
            cursor(`pointer),
            flexDirection(`row),
            padding(px(10)),
            hover([backgroundColor(hex("eee"))]),
          ])
        );

module TodoItem = {
  let component = ReasonReact.statelessComponent("Todo");
  let make = (~item, ~onToggle, ~onEdit, _) => {
    ...component,
    render: _ =>
      <div
        className=todoItemStyle
        onClick=onToggle>
        <input
          type_="checkbox"
          style={ReactDOMRe.Style.make(~cursor="pointer", ())}
          checked={item.completed !== None}
        />
        <div style={ReactDOMRe.Style.make(~flexBasis="10px", ())} />
        <Editor value={item.text} placeholder="" onChange=onEdit />
      </div>,
  };
};

let jsNow: unit => int = [%bs.raw "function(unit) {return Date.now()}"];

let toggleItem = (item, update) => {
  let item =
    item.completed === None ?
      {...item, completed: Some(jsNow())} : {...item, completed: None};
  Api.UpdateTodo.run(item, update);
};

let editItem = (item, text, update) =>
  if (text == "") {
    Api.RemoveTodo.run(item.id, update);
  } else {
    text !== item.text ? Api.UpdateTodo.run({...item, text}, update) : ();
  };

module Todos = {
  let component = ReasonReact.reducerComponent("Todos");
  let url = "/todos";
  type state = todos;
  type action = state;
  module Data = Serde.Modules.Todos;
  let make = (~data, _) => {
    ...component,
    initialState: () => data,
    reducer: (action, state) => ReasonReact.Update(action),
    render: ({state, send}) => {
      <div>

          {List.map(
             item =>
               <TodoItem
                 item
                 key={item.id |> string_of_int}
                 onToggle={_ => toggleItem(item, send)}
                 onEdit={text => editItem(item, text, send)}
               />,
             state,
           )
           |> Array.of_list
           |> ReasonReact.array}
           <div className=todoItemStyle>
          <Editor
            value=""
            // className=Css.(style([padding2(~v=px(10), ~h=px(20))]))
            onChange={text => Api.AddTodo.run(text, send)}
            placeholder="Add an item"
            clear=true
          />
          </div>
        </div>;
        // let updateTodos = update((todos, _) => ReasonReact.Update(todos));
    },
  };
};

module LoadedTodos = Loader.F(Todos);

module Page = {
  let component = ReasonReact.statelessComponent("Page");
  let make = _children => {
    ...component,
    render: _ =>
      <div
        className=Css.(
          style([alignSelf(`center), margin(px(50)), width(px(300))])
        )>
        <div
          className=Css.(
            style([
              margin(px(20)),
              textAlign(`center),
              fontSize(em(1.3)),
            ])
          )>
          {str("A Nice Todo List")}
        </div>
        <LoadedTodos />
      </div>,
  };
};

Devtools.register();

ReactDOMRe.renderToElementWithId(<Page />, "index");