/* main.js
 * Heiher <root@heiher.info>
 */

function get_base_url () {
	return window.location.protocol + '//' +
		window.location.hostname;
}

function load_file_list () {
	$.ajax({ url: '/fb', type: 'GET',
		success: function (data) {
			var content = '';
			var files = data.split ('\r\n');

			content += "<table border='0'>";
			for (var i=0; i<files.length; i++) {
				if ('' == files[i]) continue;
				content += "<tr>";
				content += "<th><a style='cursor:pointer' " +
							"title='点击查看此文件的详细信息' " +
							"onclick=\"query_file_info ('" + files[i] + "')\">" +
							files[i] + "</a></th>";
				content += "<td> &#10132;&nbsp; </td>";
				content += "<td><a target='_blank' href='/fb/" + files[i] + "'>" +
							get_base_url () + "/fb/" + files[i] + "</a></td>";
				content += "</tr>";
			}
			content += "</table>";

			$('#file-list').html (content);

			setTimeout (load_file_list, 2000);
		}});
}

function query_file_info (file) {
	$.ajax({ url: '/fb/query?file=' + file, type: 'GET',
		success: function (data) {
			alert (data);
	}});
}

function switch_opt_symbol () {
	var idx = $(this).prop ('id').substring (4, 5);
	if ('' == $(this).val ()) {
		$('#fo' + idx).html ('&#10010;');
	} else {
		$('#fn' + idx).html ($(this).val ());
		$('#fo' + idx).html ('&#10008;');
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

	$('#file-upload input[type=file]').bind ('change focus click', switch_opt_symbol);
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
			alert (xhr.statusText);
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

