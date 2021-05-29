<?php

class AddStuffForm extends Model
{

    public string $name;
    public string $price;
    public string $description;
    public string $currency;

    public function rules(): array
    {
        return [
            'name'        => [self::RULE_REQUIRED, [self::RULE_MAX, 'max' => 256], [self::RULE_UNIQUE, 'class' => Stuff::class]],
            'price'       => [self::RULE_REQUIRED],
            'description' => [[self::RULE_MAX, 'max' => 2000]],
            'currency'    => [[self::RULE_MAX, 'max' => 256]]
        ];
    }

    public function getLabels(): array
    {
        return [];
    }

    public function add()
    {
        // Create new stuff
        $stuff = new Stuff();

        // Check if the stuff's price is numeric
        if (!is_numeric($this->price)) {
            $this->addError('price', 'Giá là số không phải chữ nhoa, đây là giờ toán không phải giờ ngữ vănn');
            return false;
        }

        // Insert info
        $stuff->name        = $this->name;
        $stuff->price       = $this->price;
        $stuff->description = $this->description;
        $stuff->currency    = $this->currency;
        
        // Return new stuff~
        $stuff->save();
        return true;
    }

};