<?php

class AddUserForm extends Model
{

    public string $email     = '';
    public string $password  = '';
    public string $firstName = '';
    public string $lastName  = '';
    public int    $privilege = User::PRIVILEGE_USER;

    public function rules(): array
    {
        return [
            'firstName'       => [self::RULE_REQUIRED],
            'lastName'        => [self::RULE_REQUIRED],
            'email'           => [self::RULE_REQUIRED, self::RULE_EMAIL, [self::RULE_UNIQUE, 'class' => User::class]],
            'password'        => [self::RULE_REQUIRED, [self::RULE_MIN, 'min' => 8], [self::RULE_MAX, 'max' => 32]],
            'privilege'       => [self::RULE_REQUIRED],
        ];
    }

    public function getLabels(): array
    {
        return [];
    }

    public function add() 
    {
        // Create new user
        $user = new User();

        // Modify & update info
        $user->firstName = $this->firstName;
        $user->lastName  = $this->lastName;
        $user->email     = $this->email;
        $user->password  = $this->password;
        $user->privilege = $this->privilege;

        // Return true
        $user->save();
        return true;
    }

};