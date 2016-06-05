CREATE TABLE emp_sal (
lastname VARCHAR(10) ,
firstname VARCHAR(10),
salary DECIMAL(7, 2),
CONSTRAINT unique_cn UNIQUE (lastname, firstname)
);
