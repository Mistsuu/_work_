<?php

abstract class DBModel extends Model
{

    abstract public static function getTableName(): string;

    abstract public static function getAttributes(): array;

    abstract public static function getPrimaryKey(): string;

    public function save()
    {
        $tableName  = $this->getTableName();
        $attributes = $this->getAttributes();

        // If the same item with same ID
        // is found in the database,
        // it means a rewrite, not insert
        $primaryKey   = $this->getPrimaryKey();
        $primaryValue = $this->{$primaryKey};
        if (isset($this->{$primaryValue}) || $this->findOne([$primaryKey => $primaryValue])) {
            $params     = array_map(fn($attr) => ":$attr", $attributes);
            $statement  = self::prepare("UPDATE $tableName SET " . implode(', ', array_map(fn($attribute) => "$attribute=:$attribute", $attributes)) . " WHERE $primaryKey=$primaryValue");
        }
        else {
            $params     = array_map(fn($attr) => ":$attr", $attributes);
            $statement  = self::prepare("INSERT INTO $tableName (" . implode(',', $attributes) . ") VALUES(" . implode(',', $params) . ")");
        }

        // Bind variables to statement
        foreach($attributes as $attribute) {
            $statement->bindValue(":$attribute", $this->{$attribute});
        }

        // Execute and return
        $statement->execute();
        return true;
    }

    public static function prepare($SQLCommandTemplate)
    {
        return Application::$app->database->dataObj->prepare($SQLCommandTemplate);
    }

    public static function findOne($where)
    {
        $tableName          = static::getTableName();
        $attributes         = array_keys($where);
        $SQLCommandTemplate = implode("AND ", array_map(fn($attribute) => "$attribute = :$attribute", $attributes)); // 'AND '.join([strings look like 'abc = :abc'])  => 'abc = :abc AND def = :def ...'
        $statement          = self::prepare("SELECT * FROM $tableName WHERE $SQLCommandTemplate");
        foreach ($where as $attribute => $value) {
            $statement->bindValue(":$attribute", $value);
        }

        $statement->execute();
        return $statement->fetchObject(static::class);
    }
    
    public static function findAll()
    {
        $tableName = static::getTableName();
        $statement = self::prepare("SELECT * FROM $tableName");
        $statement->execute();
        return $statement->fetchAll(PDO::FETCH_CLASS, static::class);
    }

    public static function removeOne($where)
    {
        $tableName          = static::getTableName();
        $attributes         = array_keys($where);
        $SQLCommandTemplate = implode("AND ", array_map(fn($attribute) => "$attribute = :$attribute", $attributes)); // 'AND '.join([strings look like 'abc = :abc'])  => 'abc = :abc AND def = :def ...'
        $statement          = self::prepare("DELETE FROM $tableName WHERE $SQLCommandTemplate");
        foreach ($where as $attribute => $value) {
            $statement->bindValue(":$attribute", $value);
        }

        $statement->execute();
    }

}