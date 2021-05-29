<?php
/*
*   loader.php():
*       Set environment variables and create autoloader.
*/

/*
    ------------------ Enviroment Vars --------------------------
*/
// Setup global variables for databases
$_ENV['DB_DSN']      = 'mysql:host=localhost;port=3306;dbname=sushop';
$_ENV['DB_USERNAME'] = 'debian-sys-maint';
$_ENV['DB_PASSWORD'] = 'mcZPxnqZ64ahkunr';

// Setup global variable for working directory
$_ENV['WORKDIR'] = dirname(__DIR__);

// Setup global variable for autoloader directory
$_ENV['AUTOLOAD'] = [
    'controllers',
    'core',
    'core/form',
    'core/middlewares',
    'core/exception',
    'views/form',
    'models'
];

/*
    -------------------- Autoloader -----------------------------
*/
// Register a function for autoload 
spl_autoload_register('autoLoader');

function autoLoader($className)
{
    // echo "Loading $className...<br>";
    $autoloadFolders = $_ENV['AUTOLOAD'];

    foreach ($autoloadFolders as $folderName) {
        // Checks if the file exists
        $path = $_ENV['WORKDIR']."/$folderName/$className.php";
        // echo "Checking $path... ";
        if (!file_exists($path)) {
            // echo " not here.<br>";
            continue;
        }
        // echo " here.<br>";

        // Include the file and return
        require_once $path;
        return true;
    }

    return false;
}

/*
    ----------------- Miscancellous ------------------------------
*/
function unescapeJSON($data)
{
    return str_replace('&#34;', '"', $data);
}