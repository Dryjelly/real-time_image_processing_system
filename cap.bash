#!/bin/bash

args=$1

case $args in
	"crop") #open two fullscreen windows
		eog --fullscreen /image/face_id.gif&
		#wait until windows are actually created
		while [ 1 ]; do
   			found=$(wmctrl -l |grep 'face_id.gif');
   			if ! [[ -z "$found" ]]; then
				break;
   			fi;
		done;
		#move first win to the left
		wmctrl -r 'face_id.gif' -e 1,1920,0,-1,-1
		rm /facenet/data/test/anchor/*
		export PYTHONPATH=$PYTHONPATH:/facenet
		python /facenet/Image_Dataset_Generator.py
		sleep 3
		wmctrl -c face_id.gif;;

	"train")eog --fullscreen /image/image_processing.gif&
		#wait until windows are actually created
		while [ 1 ]; do
   			found=$(wmctrl -l |grep 'image_processing.gif');
   			if ! [[ -z "$found" ]]; then
      				break;
   			fi;
		done;
		#move first win to the left
		wmctrl -r 'image_processing.gif' -e 1,1920,0,-1,-1

		python /facenet/src/classifier.py TRAIN /facenet/data/test/ /facenet/model_checkpoints/20180402-114759/20180402-114759.pb /facenet/model_checkpoints/my_classifier_1.pkl --batch_size 1000
		sleep 3
		wmctrl -c image_processing.gif;;

	"object") /yolo/darknet/darknet detector demo /yolo/darknet/train_file/obj.data /yolo/darknet/train_file/yolov3.cfg /yolo/darknet/final_dataset_backup_capston/yolov3_16000.weights&

		while [ 1 ]; do
			sleep 2
   			found=$(wmctrl -l |grep 'Demo');
   			if ! [[ -z "$found" ]]; then
      				break;
   			fi;
		done;
		#move first win to the left
		wmctrl -r 'Demo' -e 1,1920,0,1920,1080
		;;

	"face") export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/esdl/anaconda3/envs/py36_tf12/lib # your library path
		export PYTHONPATH=$PYTHONPATH:/facenet/src
		export PYTHONPATH=$PYTHONPATH:/facenet/contributed
		/facenet/contributed/yolo_python &
		sleep 10
		/yolo/darknet/darknet detector demo /yolo/darknet/face_test/face_obj.data /yolo/darknet/face_test/yolov3-face.cfg /yolo/darknet/face_test/yolov3-wider_16000.weights&

		while [ 1 ]; do
			sleep 2
   			found=$(wmctrl -l |grep 'Demo');
   			if ! [[ -z "$found" ]]; then
      				break;
   			fi;
		done;
		#move first win to the left
		wmctrl -r 'Demo' -e 1,1920,0,1920,1080
		;;

	"socket") export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/esdl/anaconda3/envs/py36_tf12/lib # your library path
		  export PYTHONPATH=$PYTHONPATH:/facenet/src
		  export PYTHONPATH=$PYTHONPATH:/facenet/contributed
		  python /facenet/contributed/cap_edit_vol2.py
esac
