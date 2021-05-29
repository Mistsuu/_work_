<?php

class Session
{

    protected const FLASH_KEY = 'flash_messages';

    public function __construct()
    {
        session_start();
        $_SESSION[self::FLASH_KEY] = $_SESSION[self::FLASH_KEY] ?? [];
    }

    public function setFlash($key, $message)
    {
        $_SESSION[self::FLASH_KEY][$key] = [
            'toBeRemoved' => false,
            'value'       => $message
        ];
    }

    public function getFlash($key)
    {
        return $_SESSION[self::FLASH_KEY][$key]['value'] ?? '';
    }

    public function popFlash($key)
    {
        // Remove flash
        $flashMessages = $_SESSION[self::FLASH_KEY] ?? [];
        $flashMessages[$key]['toBeRemoved'] = true;
        $_SESSION[self::FLASH_KEY] = $flashMessages;

        // Return value
        return $this->getFlash($key);
    }

    public function __destruct()
    {
        // Iterate over marked to be removed message
        $flashMessages = $_SESSION[self::FLASH_KEY] ?? [];
        foreach ($flashMessages as $key => &$flashMessage) {
            if ($flashMessage['toBeRemoved']) {
                unset($flashMessages[$key]);
            }
        }
        $_SESSION[self::FLASH_KEY] = $flashMessages;
    }

    public function set($key, $value)
    {
        $_SESSION[$key] = $value;
    }

    public function get($key)
    {
        return $_SESSION[$key] ?? '';
    }

    public function remove($key)
    {
        unset($_SESSION[$key]);
    }

    public function isGuest()
    {
        return !isset($_SESSION['user']);
    }

    public function isRegularUser()
    {
        return isset($_SESSION['user']) && !isset($_SESSION['isAdmin']);
    }

};