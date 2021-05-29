<?php

class Application 
{
    
    // Variables that handles the route of user.
    public Router   $router;
    public Request  $request;
    public Response $response;
    public Session  $session;

    // Variables that handles data
    public Database $database;

    // Refers to self, Controller, View.
    public static Application $app;
    public        ?Controller $controller = null;
    public        View        $view;

    // Working directory
    public static string $ROOT_DIR;

    public function __construct(array $configs) 
    {
        // ----------------------------- Values ----------------------------------
        // Create objects...
        $this->request  = new Request();
        $this->response = new Response();
        $this->session  = new Session();
        $this->router   = new Router($this->request, $this->response, $this->session);
        $this->database = new Database($configs['db']);
        $this->view     = new View();

        // Setting static global values
        self::$ROOT_DIR   = $configs['rootdir'];
        self::$app        = $this;
    }

    public function run() 
    {
        try {
            echo $this->router->resolve();
        } catch (Exception $e) {
            $this->response->setStatusCode($e->getCode());
            echo $this->view->renderView('_error', [
                'exception' => $e
            ]);
        }
    }

};