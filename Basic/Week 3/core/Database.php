<?php

class Database
{

    public PDO $dataObj;

    public function __construct(array $config)
    {
        $dataSourceName = $config['dsn']  ?? '';
        $username       = $config['user'] ?? '';
        $password       = $config['pass'] ?? '';

        $this->dataObj = new PDO($dataSourceName, $username, $password);
        $this->dataObj->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    }

    protected function log($message)
    {
        echo '[' . date('Y-m-d H:i:s') . '] - ' . $message . "\n";
    }

    public function prepare($SQLCommandTemplate)
    {
        return $this->dataObj->prepare($SQLCommandTemplate);
    }

};