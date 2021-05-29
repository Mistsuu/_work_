<?php

class View
{

    public string $title           = '';
    public array  $css             = [];
    public array  $js              = [];
    public array  $onLoadBodyFuncs = [];

    public function renderView($view, $params=[])
    {
        $viewContent   = $this->renderOnlyView($view, $params);
        $layoutContent = $this->layoutContent();
        return str_replace('{{content}}', $viewContent, $layoutContent);
    }

    protected function layoutContent() 
    {
        // Get the layout value.
        $layout = Application::$app->controller ? Application::$app->controller->layout : Controller::$defaultLayout;

        // Return the buffered layout of the website.
        ob_start();
        include_once Application::$ROOT_DIR."/views/layouts/$layout.php";
        return ob_get_clean();
    }

    protected function renderOnlyView($view, $params=[]) 
    {
        // Convert key to variable
        foreach ($params as $key => $value) {
            $$key = $value;
        }

        // Return the buffered content of the website.
        ob_start();
        include_once Application::$ROOT_DIR."/views/$view.php";
        return ob_get_clean();
    }

    public function setTitle(string $title)
    {
        $this->title = $title;
    }

    public function addCssFiles(array $css)
    {
        $this->css = $css;
    }

    public function addJSFiles(array $js)
    {
        $this->js = $js;
    }

    public function addOnloadBodyFuncs(array $funcs)
    {
        $this->onLoadBodyFuncs = $funcs;
    }

    public function getTitle()
    {
        return $this->title;
    }

    public function loadCssFilesAsHTML()
    {
        $HTMLStatements = "";
        foreach ($this->css as $cssFile)
            $HTMLStatements .= "<link rel=\"stylesheet\" href=\"css/$cssFile.css\">\n";
        return $HTMLStatements;
    }

    public function loadJSFilesAsHTML()
    {
        $HTMLStatements = "";
        foreach ($this->js as $jsFile)
            $HTMLStatements .= "<script src=\"js/$jsFile.js\"></script>\n";
        return $HTMLStatements;
    }

    public function loadOnloadBodyFuncsAsHTML()
    {
        $HTMLStatements = "";
        foreach ($this->onLoadBodyFuncs as $func => $args) {
            $HTMLStatements .= " $func(";
            foreach ($args as $arg) {
                $HTMLStatements .= "$arg,";
            }
            $HTMLStatements .= ");";
        }
        return $HTMLStatements;
    }

};