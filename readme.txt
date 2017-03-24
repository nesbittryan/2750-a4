CIS*2750 Assignment 3
--------------------------------
Created By: Ryan Nesbitt
ID: 0915819

Assumptions/Limitations I made about the assignment:
    That adding style elements to help improve website look into the config files was okay
    That I can set up the usage for my programs in a way I see fit
    Text entered by users can not contain double quotes, as they will mess up the php

    That the TA marking this has mercy for my soul

HOW TO USE EACH EXECUTABLE BY COMMAND LINE (ignore if only using web interface):
    Site converter C file (without the quotes):

        "./<path/>sc <filename.wpml> Streamname MessageNum Username"

        If you are not using hidden inputs, then these three values can be replaced
        by inputting STREAM_NULL MESSAGE_NULL and NAME_NULL

        ie. " ./<path/>sc <filename.wpml> STREAM_NULL MESSAGE_NULL NAME_NULL "

        I did it this way to pass around the username, current stream and current message num
        while still using post.

    Add Author (without the quotes):
        " ./<path/>addauthor flag[add|remove] username[multiple words allowed] streamList[, seperated, no space]"
        ie. "./addauthor add Ryan Nesbitt s1,s2,s3,s4"

    Post (without the quotes):
        " ./<path/post streamname username[multiple words] MESSAGE_TEXT Message[multiple words]"
        ie. "./post s1 Ryan Nesbitt MESSAGE_TEXT Hello this is my message"

    Python view (without the quotes):

        This way is used when first calling it, and brings you to the select stream menu
        "./<path/>view.py Username[multiple words allowed]"
        ie "./view.py Ryan Nesbitt"

        This way is used when a stream is already selected.
            streamname = the name of the current stream
            readNum = the index of the message currently on, -1 resets list, -2 marks all as read
            sortFlag = method of viewing, 0 = date, 1 = by Author
        "./<path/>view.py STREAM_NAME streamname readNum sortFlag Username[multiple words allowed]"
        ie "./view.py STREAM_NAME s1 0 0 Ryan Nesbitt"
