## List datasets in bigquery
bq ls <project_name>:

## List tables in dataset
bq ls <project_name>:<dataset>

## Columns for matching tables
 for table in $(bq ls <project_name>:<schema> | awk '{print $1}' | grep '^matching_criteria'); do
    echo "=== $table ==="
    bq show --format=prettyjson <project_name>:<dataset>.$table | jq '.schema.fields[] | {name, type, mode}'
    echo ""
  done
