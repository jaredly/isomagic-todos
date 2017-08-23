open Lwt;
open Cohttp;
open Cohttp_lwt_unix;

let module CoServer = Cohttp_lwt_unix.Server;

let json data => {
  let body = Yojson.Safe.to_string data;
  let module H = Cohttp.Header;
  let headers = H.init();
  let headers = H.add headers "content-type" "application/json";
  CoServer.respond_string ::headers status::`OK ::body ()
};

let serveStatic base req body path next => {
  let fname = (Filename.concat base path);
  if (Sys.file_exists fname) {
    if (Sys.is_directory fname) {
      if (Sys.file_exists (Filename.concat fname "index.html")) {
        CoServer.respond_file fname::(Filename.concat fname "index.html") ();
      } else {
        next (); /* TODO do a directory listing maybe */
      }
    } else {
      CoServer.respond_file ::fname ();
    }
  } else if (Sys.file_exists (fname ^ ".html")) {
    CoServer.respond_file fname::(fname ^ ".html") ();
  } else {
    next ()
  }
};

let handlers = ref [];

let all path fn => handlers := [
  fun req body path' next => {
    if (path == path') {
      fn req body next
    } else {
      next ()
    }
  },
  ...!handlers
];

let handle meth path fn => handlers := [
  fun req body path' next => {
    if (meth == Request.meth req && path == path') {
      fn req body next
    } else {
      next ()
    }
  },
  ...!handlers
];

let handle_prefix meth path fn => {
  let size = (String.length path);
  handlers := [
    fun req body path' next => {
      let pathLen = (String.length path');
      if (meth == Request.meth req && pathLen >= size && (String.sub path' 0 size) == path) {
        fn req body (String.sub path' size (pathLen - size)) next
      } else {
        next ()
      }
    },
    ...!handlers
  ]
};

let maybe_parse text => try(Some (Yojson.Safe.from_string text)) {
| _ => None
};

let get = handle `GET;
let post = handle `POST;
let delete = handle `DELETE;
let get_prefix = handle_prefix `GET;
let post_prefix = handle_prefix `POST;
let delete_prefix = handle_prefix `DELETE;




let module Get (Config: SApi.Get) (Handler: {let handle: Cohttp.Request.t => Config.response; }) => {
  handle `GET Config.path (fun req _ _ => json (Config.response__to_yojson (Handler.handle req)));
};

let module Let_syntax = {
  let bind value ::f => Lwt.bind value f;
  let map value ::f => Lwt.map f value;
};

let module Post (Config: SApi.Post) (Handler: {let handle: Cohttp.Request.t => Config.request => Config.response; }) => {
  handle `POST Config.path (fun req body _ => {
    [%await let body = Cohttp_lwt_body.to_string body];

    [%guard let Some(data) = maybe_parse body]
    [@else CoServer.respond_string status::`Bad_request body::"" ()];

    [%guard let Some(data) = Config.request__from_yojson data]
    [@else CoServer.respond_string status::`Bad_request body::"" ()];

    json (Config.response__to_yojson (Handler.handle req data))
  });
};



let server port => {
  let handlers = List.rev (!handlers);

  let rec next req body path items () => {
    switch items {
    | [] => CoServer.respond_string status::`Not_found body::"Not found" ()
    | [fn, ...rest] => fn req body path (next req body path rest)
    }
  };

  let callback _conn req body => {
    let path = req |> Request.uri |> Uri.path;
    next req body path handlers ();
  };

  CoServer.create mode::(`TCP (`Port port)) (CoServer.make ::callback ())
};

let listen port => {
  Printf.printf "Listening at http://localhost:%d" port;
  print_newline ();
  ignore (Lwt_main.run (server port));
};