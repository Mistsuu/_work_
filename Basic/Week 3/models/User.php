<?php

class User extends DBModel
{

    public const PRIVILEGE_ADMIN = 1;
    public const PRIVILEGE_USER  = 0;

    public string $firstName        = '';
    public string $lastName         = '';
    public string $email            = '';
    public string $password         = '';
    public string $passwordConfirm  = '';
    public int    $privilege        = self::PRIVILEGE_USER;

    public function save($hashPassword=true) 
    {
        if ($hashPassword)
            $this->password = password_hash($this->password, PASSWORD_DEFAULT);
        return parent::save();
    }

    public function getLabels(): array
    {
        return [
            'firstName'       => 'Họ',
            'lastName'        => 'Tên',
            'email'           => 'Email',
            'password'        => 'Mật khẩu',
            'passwordConfirm' => 'Giống trên',
        ];
    }

    public static function getTableName(): string
    {
        return 'users';
    }

    public static function getAttributes(): array
    {
        return [
            'firstName',
            'lastName',
            'email',
            'password',
            'privilege'
        ];
    }

    public static function getPrimaryKey(): string
    {
        return 'id';
    }

    public function rules(): array
    {
        return [
            'firstName'       => [self::RULE_REQUIRED],
            'lastName'        => [self::RULE_REQUIRED],
            'email'           => [self::RULE_REQUIRED, self::RULE_EMAIL, [self::RULE_UNIQUE, 'class' => self::class]],
            'password'        => [self::RULE_REQUIRED, [self::RULE_MIN, 'min' => 8], [self::RULE_MAX, 'max' => 32]],
            'passwordConfirm' => [self::RULE_REQUIRED, [self::RULE_MATCH, 'match' => 'password']]
        ];
    }

    public function getDisplayName(): string
    {
        return $this->firstName.' '.$this->lastName;
    }

};