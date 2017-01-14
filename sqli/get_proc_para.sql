SELECT 
	argument_name PARAMETER_NAME,
	POSITION POS,
	data_type TYPE,
	in_out INOUT,
	CHAR_LENGTH CHAR_OCTET_LEN,
	DATA_PRECISION NUM_PRECISION,
	DATA_SCALE SCALE 
from   all_arguments
WHERE object_id=(
	select object_id 
	from all_objects 
	where object_name='MAKE_SEATS')
	ORDER BY POSITION;
