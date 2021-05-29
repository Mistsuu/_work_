<?php

require_once "loader.php";

// Configs for the server...
$configs = [
    // Root folder for the project
    'rootdir'    => $_ENV['WORKDIR'],

    // Database configs
    'db' => [
        'dsn'  => $_ENV['DB_DSN'],
        'user' => $_ENV['DB_USERNAME'],
        'pass' => $_ENV['DB_PASSWORD']
    ]
];

// Create application object
$app = new Application($configs);

// List of GET routing options
$app->router->get('/',                   [SiteController::class,  'home']);
$app->router->get('/contact',            [SiteController::class,  'contact']);
$app->router->get('/login',              [AuthController::class,  'login']);
$app->router->get('/register',           [AuthController::class,  'register']);
$app->router->get('/logout',             [AuthController::class,  'logout']);
$app->router->get('/profile',            [AuthController::class,  'profile']);
$app->router->get('/viewItem',           [SiteController::class,  'viewItem']);
$app->router->get('/viewCart',           [AuthController::class,  'viewCart']);
$app->router->get('/editProfile',        [AuthController::class,  'editProfile']);
$app->router->get('/checkout',           [AuthController::class,  'checkout']);
$app->router->get('/getUserStatus',      [AjaxController::class,  'getUserStatus']);
$app->router->get('/getUsername',        [AjaxController::class,  'getUsername']);
$app->router->get('/getFlashMessage',    [AjaxController::class,  'getFlashMessage']);
$app->router->get('/getCartIDs',         [AjaxController::class,  'getCartIDs']);
$app->router->get('/getStuffByID',       [AjaxController::class,  'getStuffByID']);
$app->router->get('/getAllStuffs',       [AjaxController::class,  'getAllStuffs']);
$app->router->get('/getUser',            [AjaxController::class,  'getUser']);
$app->router->get('/addToCart',          [AjaxController::class,  'addToCart']);
$app->router->get('/removeFromCart',     [AjaxController::class,  'removeFromCart']);
$app->router->get('/editStuffs',         [AdminController::class, 'editStuffs']);
$app->router->get('/editUsers',          [AdminController::class, 'editUsers']);

// List of POST routing options
$app->router->post('/contact',           [SiteController::class,  'contact']);
$app->router->post('/login',             [AuthController::class,  'login']);
$app->router->post('/register',          [AuthController::class,  'register']);
$app->router->post('/editProfile',       [AuthController::class,  'editProfile']);
$app->router->post('/getAllUsers',       [AjaxController::class,  'getAllUsers']);
$app->router->post('/modifyStuff',       [AjaxController::class,  'modifyStuff']);
$app->router->post('/addStuff',          [AjaxController::class,  'addStuff']);
$app->router->post('/removeStuff',       [AjaxController::class,  'removeStuff']);
$app->router->post('/removeUser',        [AjaxController::class,  'removeUser']);
$app->router->post('/addUser',           [AjaxController::class,  'addUser']);
$app->router->post('/modifyUser',        [AjaxController::class,  'modifyUser']);

// Start application
$app->run();