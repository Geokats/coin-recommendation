#! /bin/bash
conf_template=""
init=""
assign=""
update=""

while getopts "c:i:a:u:" opt; do
  case "$opt" in
    c)conf_template=$OPTARG
      ;;
    i)
      init=$OPTARG
      ;;
    a)
      assign=$OPTARG
      ;;
    u)
      update=$OPTARG
      ;;
  esac
done

shift $((OPTIND-1))
[ "${1:-}" = "--" ] && shift

output_file="temp_out.txt"
conf_file="current_configuration.conf"

results_file="${init}_${assign}_${update}.csv"
echo "k, time, silhouette" > $results_file;

clusters=(5 10 20 30 40 50 60 75 100 200 300 400)

for k in ${clusters[*]}; do
  cat $conf_template > $conf_file
  echo "initialise: ${init}" >> $conf_file
  echo "assign: ${assign}" >> $conf_file
  echo "update: ${update}" >> $conf_file
  echo "number_of_clusters: $k" >> $conf_file

  ../../bin/cluster -i ../../data/twitter_dataset_small_v2.csv -c $conf_file -o $output_file

  time=$(grep "Clustering time: " $output_file)
  time=${time:17:6}

  silhouette=$(grep "Average Silhouette: " $output_file)
  silhouette=${silhouette:20:6}


  echo "k = $k, time = $time, silhouette = $silhouette"
  echo "$k, $time, $silhouette" >> $results_file
done

rm $conf_file
rm $output_file
