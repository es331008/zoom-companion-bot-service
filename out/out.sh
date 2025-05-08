#!/bin/bash

# Filename list with full structure: meetingId_epoch.json
files=(
  "3021457398_1746600000.json"
  "6182503841_1746636000.json"
  "8849123427_1746650400.json"
  "7123948576_1746513600.json"
  "9943012883_1746553200.json"
  "5849302871_1746564000.json"
  "1604927384_1746571200.json"
  "4593827163_1746592800.json"
  "3829401283_1746192000.json"
  "9273846572_1746278400.json"
  "4938572019_1746314400.json"
  "8401293847_1746361200.json"
  "2918374560_1746386400.json"
  "5729301846_1746462000.json"
  "1049283746_1746487200.json"
)

for filename in "${files[@]}"; do
  base_name=$(basename "$filename" .json)

  meeting_id=$(echo "$base_name" | cut -d'_' -f1)
  epoch_time=$(echo "$base_name" | cut -d'_' -f2)

  echo "Creating $filename (ID: $meeting_id, Time: $epoch_time)"

  cat <<EOF > "$filename"
{
  "meetingNumber": "$meeting_id",
  "meetingTopic": "Demo Meeting for $meeting_id",
  "start_time": $epoch_time
}
EOF
done

echo "✅ Created ${#files[@]} files in ./out/"
