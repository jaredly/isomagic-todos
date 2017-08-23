
open! Types;

let str = ReasonReact.stringToElement;
let style = ReactDOMRe.Style.make;

let module TodoItem = {
  let component = ReasonReact.statelessComponent "Todo";
  let make ::item ::onToggle ::onEdit _ => {
    ...component,
    render: fun _ => <div
      className=(Glamor.(css [
        backgroundColor "white",
        cursor "pointer",
        flexDirection "row",
        padding "10px",
        Selector ":hover" [
          backgroundColor "#eee"
        ]
      ]))
      onClick=onToggle
    >
      <input _type="checkbox" checked=(item.completed !== None |> Js.Boolean.to_js_boolean) />
      <div style=(style flexBasis::"10px" ()) />
      <Editor value=(item.text) placeholder="" onChange=onEdit />
    </div>
  }
};

let jsNow: unit => int = [%bs.raw "function() {return Date.now()}"];

let toggleItem item update => {
  let item = item.completed === None
    ? {...item, completed: Some (jsNow())}
    : {...item, completed: None};
  Api.updateItem item update
};
let editItem item text update => if (text == "") {
  Api.removeItem item.id update
} else {
  text !== item.text ? Api.updateItem {...item, text} update : ()
};

let module Todos = {
  let component = ReasonReact.statefulComponent "Todos";
  let url = "/todos";
  type state = todos;
  type data = todos;
  let make ::data _ => {
    ...component,
    initialState: fun () => data,
    render: fun {state, update} => {
      let updateTodos = (update (fun todos _ => ReasonReact.Update todos));
      <div>
        (List.map
          (fun item =>
            <TodoItem
              item
              key=(item.id |> string_of_int)
              onToggle=(fun _ => toggleItem item updateTodos)
              onEdit=(fun text => editItem item text updateTodos)
            />
          ) state
          |> Array.of_list |> ReasonReact.arrayToElement)
        <Editor
          value=""
          className=(Glamor.(css[padding "10px 20px"]))
          onChange=(fun text => Api.addItem text updateTodos)
          placeholder="Add an item"
          clear=true
        />
      </div>
    }
  }
};

let module LoadedTodos = Loader.F Todos;

let module Page = {
  let component = ReasonReact.statelessComponent "Page";
  let make _children => {
    ...component,
    render: fun _ =>
      <div className=(Glamor.(css [
        alignSelf "center",
        margin "50px",
        width "300px"
      ]))>
      <div className=(Glamor.(css [
        margin "20px",
        textAlign "center",
        fontSize "1.3em",
      ]))>
        (str "A Nice Todo List")
        </div>
        <LoadedTodos />
      </div>
  };
};

Devtools.register();
ReactDOMRe.renderToElementWithId <Page /> "index";
