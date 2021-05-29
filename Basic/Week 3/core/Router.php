<?php

class Router 
{

    protected array    $routes = [];
    public    Request  $request;
    public    Response $response;
    public    Session  $session;

    public function __construct(Request $request, Response $response, Session $session) 
    {
        $this->request  = $request;
        $this->response = $response;
        $this->session  = $session;
    }

    public function get($path, $callback) 
    {
        $this->routes['get'][$path] = $callback;
    }

    public function post($path, $callback) 
    {
        $this->routes['post'][$path] = $callback;
    }

    public function resolve() 
    {
        $path     = $this->request->getPath();
        $method   = $this->request->getMethod();
        $callback = $this->routes[$method][$path] ?? false;

        // Not found -> Set code to 404, return NOT FOUND.
        if (!$callback) {
            throw new NotFoundException();
        }

        // String -> Path to a PHP file.
        if (is_string($callback)) {
            return Application::$app->view->renderView($callback);
        }

        // Array -> Instantiate an object of type.
        if (is_array($callback)) {
            $controller                   = new $callback[0]();
            Application::$app->controller = $controller;
            $controller->action           = $callback[1];        // Refers to the URL we're heading to.
            $callback[0]                  = $controller;
            
            foreach ($controller->getMiddlewares() as $middleware) {
                $middleware->execute();
            }
        }

        // Calls the specified functions
        return call_user_func($callback, $this->request, $this->response, $this->session);
    }

};