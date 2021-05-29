<?php

class Stuff extends DBModel
{

    public string $name;
    public float  $price;
    public string $description;
    public string $currency;

    public static function getPrimaryKey(): string
    {
        return 'id';
    }

    public static function getTableName(): string
    {
        return 'stuffs';
    }

    public static function getAttributes(): array
    {
        return [
            'name',
            'price',
            'description',
            'currency',
        ];
    }

    public function rules(): array
    {
        return [
            'name'        => [self::RULE_REQUIRED, [self::RULE_MAX, 'max' => 256], [self::RULE_UNIQUE, 'class' => self::class]],
            'price'       => [self::RULE_REQUIRED],
            'description' => [[self::RULE_MAX, 'max' => 2000]],
            'currency'    => [[self::RULE_MAX, 'max' => 256]]
        ];
    }

    public function getLabels(): array
    {
        return [
            'name'        => 'Name',
            'price'       => 'Price',
            'description' => 'Description',
            'currency'    => 'Currency'
        ];
    }

    public function getPrice(): int
    {
        return $this->price;
    }

    public function getName(): string
    {
        return $this->name;
    }

    public function getDescription(): string
    {
        return $this->description;
    }

    public function getCurrency(): string
    {
        return $this->currency;
    }

};