<?php

abstract class Model
{
    
    // Rules the data has to follow
    public const RULE_REQUIRED = 'required';
    public const RULE_EMAIL    = 'email';
    public const RULE_MIN      = 'min';
    public const RULE_MAX      = 'max';
    public const RULE_MATCH    = 'match';
    public const RULE_UNIQUE   = 'unique';

    // Contains the message errors
    public array $errors = [];

    public function loadData($data)
    {
        foreach ($data as $key => $value) {
            if (property_exists($this, $key)) {
                $this->{$key} = $value;
            }
        }
    }

    abstract public function rules(): array;

    public function getLabels(): array
    {
        return [];
    }

    public function getLabel(string $attribute): string
    {
        return $this->getLabels()[$attribute] ?? $attribute;
    }

    public function validate()
    {
        foreach ($this->rules() as $attribute => $rules) {
            // Get value
            $value = $this->{$attribute};
            
            // Query for each rule
            foreach ($rules as $rule) {
                // Retrieve rule name
                $ruleName = $rule;
                if (is_array($ruleName)) {
                    $ruleName = $rule[0];
                }

                // Check conditions
                if ($ruleName == self::RULE_REQUIRED && (!$value && !is_numeric($value))) {
                    $this->addErrorForRule($attribute, self::RULE_REQUIRED);
                }
                if ($ruleName == self::RULE_EMAIL && $value != '' && !filter_var($value, FILTER_VALIDATE_EMAIL)) {
                    $this->addErrorForRule($attribute, self::RULE_EMAIL);
                }
                if ($ruleName == self::RULE_MIN && $value != '' && strlen($value) < $rule['min']) {
                    $this->addErrorForRule($attribute, self::RULE_MIN, $rule);
                }
                if ($ruleName == self::RULE_MAX && $value != '' && strlen($value) > $rule['max']) {
                    $this->addErrorForRule($attribute, self::RULE_MAX, $rule);
                }
                if ($ruleName == self::RULE_MATCH && $value != $this->{$rule['match']}) {
                    $rule['match'] = $this->getLabel($rule['match']);
                    $this->addErrorForRule($attribute, self::RULE_MATCH, $rule);
                }
                if ($ruleName == self::RULE_UNIQUE) {
                    $className       = $rule['class'];
                    $uniqueAttribute = $rule['attribute'] ?? $attribute;
                    $tableName       = $className::getTableName();

                    // Executes SQL search command.
                    $statement = Application::$app->database->prepare("SELECT * FROM $tableName WHERE $uniqueAttribute = :attribute");
                    $statement->bindValue(":attribute", $value);
                    $statement->execute();

                    // Fetch data from database & checks if existed a matching result (with different ID).
                    $record = $statement->fetchObject();
                    if ($record && $record->{$className::getPrimaryKey()} != $this->{$className::getPrimaryKey()}) {
                        $this->addErrorForRule($attribute, self::RULE_UNIQUE, ['field' => $this->getLabel($attribute)]);
                    }
                }
            }
        }
        return empty($this->errors);
    }

    public function addError(string $attribute, string $message)
    {
        $this->errors[$attribute][] = $message;
    }

    public function addErrorForRule(string $attribute, string $ruleName, $params=[])
    {
        $message = $this->errorMessages()[$ruleName] ?? '';
        foreach ($params as $key => $value) {
            $message = str_replace("{{$key}}", $value, $message);
        }
        $this->errors[$attribute][] = $message;
    }

    public function hasError($attribute)
    {
        return $this->errors[$attribute] ?? false;
    }

    public function getFirstError($attribute)
    {
        return $this->errors[$attribute][0] ?? false;
    }

    public function errorMessages()
    {
        return [
            self::RULE_REQUIRED => 'Mục này bạn phải điền nha!',
            self::RULE_EMAIL    => 'Mail miếc gì loạn xì ngầu thế bạn ơi!',
            self::RULE_MIN      => 'Hơi ngắn nha bạn! Tối thiểu {min} kí tự nha!',
            self::RULE_MAX      => 'Tem tém chút bạn ưi! Ít hơn {max} kí tự nha!',
            self::RULE_MATCH    => 'Mục này phải trùng với {match} nha!',
            self::RULE_UNIQUE   => '{field} này tồn tại rồi bạn ơi!'
        ];
    }

};