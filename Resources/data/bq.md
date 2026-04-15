## List datasets in bigquery
bq ls <project_name>:

## List tables in dataset
bq ls <project_name>:<dataset>

## Show columns in a table
bq show --format=prettyjson <project_name>:<dataset>.<table_name>


## Columns for matching tables
 for table in $(bq ls <project_name>:<schema> | awk '{print $1}' | grep '^matching_criteria'); do
    echo "=== $table ==="
    bq show --format=prettyjson <project_name>:<dataset>.$table | jq '.schema.fields[] | {name, type, mode}'
    echo ""
  done

## bq Query
bq query --use_legacy_sql=false --format=prettyjson < <sql_file>.sql | jq '.'

