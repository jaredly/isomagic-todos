open Lwt;

open Cohttp;

open Cohttp_lwt_unix;

module CoServer = Cohttp_lwt_unix.Server;

let json = (data) => {
  let body = Ez2.to_string(data);
  module H = Cohttp.Header;
  let headers = H.init();
  let headers = H.add(headers, "content-type", "application/json");
  CoServer.respond_string(~headers=headers, ~status=`OK, ~body=body, ());
};

let serveStatic = (base, req, body, path, next) => {
  let fname = Filename.concat(base, path);
  if (Sys.file_exists(fname)) {
    if (Sys.is_directory(fname)) {
      if (Sys.file_exists(Filename.concat(fname, "index.html"))) {
        CoServer.respond_file(~fname=Filename.concat(fname, "index.html"), ());
      } else {
        next
          () /* TODO do a directory listing maybe */;
      };
    } else {
      CoServer.respond_file(~fname=fname, ());
    };
  } else if (Sys.file_exists(fname ++ ".html")) {
    CoServer.respond_file(~fname=fname ++ ".html", ());
  } else {
    next();
  };
};

let handlers = ref([]);

let all = (path, fn) =>
  handlers :=
    [
      (req, body, path', next) =>
        if (path == path') {
          fn(req, body, next);
        } else {
          next();
        },
      ...handlers^
    ];

let handle = (meth, path, fn) =>
    handlers :=
    [
      (req, body, path', next) =>
        if (meth == Request.meth(req) && path == path') {
          fn(req, body, next);
        } else {
          next();
        },
      ...handlers^
    ];

let handle_prefix = (meth, path, fn) => {
  let size = String.length(path);
    handlers:=
    [
      (req, body, path', next) => {
        let pathLen = String.length(path');
        if (meth == Request.meth(req) && pathLen >= size && String.sub(path', 0, size) == path) {
          fn(req, body, String.sub(path', size, pathLen - size), next);
        } else {
          next();
        };
      },
      ...handlers^
    ];
};

let maybe_parse = (text) =>
  try (Ok(
    Ezjsonm.from_string(text)
  )) {
  | _ => Error("Unable to parse")
  };

let get = handle(`GET);

let post = handle(`POST);

let delete = handle(`DELETE);

let get_prefix = handle_prefix(`GET);

let post_prefix = handle_prefix(`POST);

let delete_prefix = handle_prefix(`DELETE);

module Get = (Config: SApi.Get, Handler: {let handle: Cohttp.Request.t => Config.Response.t;}) => {
  handle(`GET, Config.path, (req, _, _) => {
    json(Config.Response.serialize(Handler.handle(req)))
  });
};

module Await = {
  let let_ = (value, ~f) => Lwt.bind(value, f);
  let map = (value, ~f) => Lwt.map(f, value);
};

module Try = {
  module Double = {
    let let_ = (value, ~f) => switch value {
      | Error(error) => error
      | Ok(v) => f(v)
    }
  };
  let let_ = (value, ~f) => switch value {
    | Error(error) => Error(error)
    | Ok(v) => f(v)
  };
  let try_ = (value, ~f) => switch value {
    | Ok(v) => Ok(v)
    | Error(error) => f(error)
  }
};

module Post =
       (
         Config: SApi.Post,
         Handler: {let handle: (Cohttp.Request.t, Config.Request.t) => Config.Response.t;}
       ) => {
  handle(
    `POST,
    Config.path,
    (req, body, _) => {
      let%Await body = Cohttp_lwt.Body.to_string(body);
      let m = {
        let%Try data = try%Try (maybe_parse(body)) {
          | message => Error(CoServer.respond_string(~status=`Bad_request, ~body="", ()))
        };
        let%Try data = try%Try (Config.Request.deserialize(data)) {
          | messages => Error(CoServer.respond_string(~status=`Bad_request, ~body="", ()))
        };
        Ok(json(Config.Response.serialize(Handler.handle(req, data))))
      };
      switch m {
        | Ok(m) => m
        | Error(m) => m
      }
    }
  );
};

let server = (port) => {
  let handlers = List.rev(handlers^);
  let rec next = (req, body, path, items, ()) => {
    print_endline("Handler next");
    switch items {
    | [] => CoServer.respond_string(~status=`Not_found, ~body="Not found", ())
    | [fn, ...rest] => fn(req, body, path, next(req, body, path, rest))
    };
  };
  let callback = (_conn, req, body) => {
    let path = req |> Request.uri |> Uri.path;
    next(req, body, path, handlers, ());
  };
  CoServer.create(~mode=`TCP(`Port(port)), CoServer.make(~callback=callback, ()));
};

let listen = (port) => {
  Printf.printf("Listening at http://localhost:%d", port);
  print_newline();
  ignore(Lwt_main.run(server(port)));
};
