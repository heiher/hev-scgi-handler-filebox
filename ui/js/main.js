/* main.js
 * Heiher <root@heiher.info>
 */

const BASE_URI = '/fb/';

function get_base_url () {
	return window.location.protocol + '//' +
		window.location.hostname;
}

function load_file_list () {
	$.ajax({ url: BASE_URI, type: 'GET',
		success: function (data) {
			var content = '';
			var files = data.split ('\r\n');

			content += "<table border='0'>";
			for (var i=0; i<files.length; i++) {
				if ('' == files[i]) continue;
				content += "<tr>";
				content += "<th width='320px'><a style='cursor:pointer' " +
							"title='点击查看此文件的详细信息' " +
							"onclick=\"query_file_info ('" + files[i] + "')\">" +
							files[i] + "</a></th>";
				content += "<td width='20px'> &#10132;&nbsp; </td>";
				content += "<td><a target='_blank' href='" + BASE_URI + files[i] + "'>" +
							get_base_url () + BASE_URI + files[i] + "</a></td>";
				content += "<td width='20px' class='del' onclick=\"delete_file ('" + files[i] + "')\"> &#10008; </td>";
				content += "</tr>";
			}
			content += "</table>";

			$('#file-list').html (content);

			setTimeout (load_file_list, 2000);
		}});
}

function query_file_info (file) {
	$.ajax({ url: BASE_URI + 'query?file=' + file, type: 'GET',
		success: function (data) {
			alert (data);
		}
	});
}

function delete_file (file) {
	var pass = prompt("请输入文件 '" + file + "' 的删除码：");
	if (null != pass) {
		$.ajax({ url: BASE_URI + 'delete?file=' + file + '&pass=' + pass, type: 'GET',
			complete: function (xhr, statusText) {
				alert (statusText);
			}
		});
	}
}

function check_files_size () {
	var size = 0, allowed_size = 1024 * 1024 * 1024;
	var files = $('.ifile');

	for (var i=0; i<files.length; i++) {
		if (files.get (i).files.length)
		  size += files.get (i).files[0].size;
	}
	if (size > allowed_size) {
		alert ('单次上传文件总长度最大 1GB！');
		return false;
	}

	return true;
}

function switch_opt_symbol () {
	var idx = $(this).prop ('id').substring (4, 5);
	if ('' == $(this).val ()) {
		$('#fo' + idx).html ('&#10010;');
	} else {
		if (check_files_size ()) {
			$('#fn' + idx).html ($(this).val ());
			$('#fo' + idx).html ('&#10008;');
		} else {
			$(this).val ('');
		}
	}
}

function reset_upload_holders () {
	var fns = $('#file-upload .filename');
	var fos = $('#file-upload .opt');
	for (var i=0; i<fns.length; i++)
	  $(fns[i]).html ('');
	for (var i=0; i<fos.length; i++)
	  $(fos[i]).html ('&#10010;');
}

function do_init () {
	load_file_list ();

	$('#file-upload').attr ('action', BASE_URI + 'upload');
	$('#file-upload input[type=file]').bind ('change', switch_opt_symbol);
}

$('#file-upload').submit (function () {
	$(this).ajaxSubmit ({
		resetForm: true,
		beforeSend: function () {
			$('#submit').prop ('disabled', true);
			$('#submit').val ('正在上传 0%...');
		},
		uploadProgress: function(event, position, total, percent) {
			$('#submit').val ('正在上传 ' + percent + '%...');
		},
		success: function () {
			$('#submit').val ('正在上传 100%...');
		},
		complete: function (xhr) {
			$('#submit').prop ('disabled', false);
			$('#submit').val ('开始上传');
			if (200 == xhr.status)
			  reset_upload_holders ();
			alert ('删除码：' + xhr.responseText);
		}
	});

	return false;
});

$('.opt').click (function () {
	var idx = $(this).prop ('id').substring (2, 3);
	var file = $('#file' + idx);
	if ('' == file.val ()) {
		file.click ();
	} else {
		file.val ('');
		$('#fn' + idx).html ('');
		$('#fo' + idx).html ('&#10010;');
	}
});

do_init ();

