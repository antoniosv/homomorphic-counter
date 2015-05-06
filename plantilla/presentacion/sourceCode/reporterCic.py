#!/usr/bin/python
#-*-coding: utf-8 -*-

from cleaner_cic import Cleaner

class Reporter:

    def __init__(self, n=50):
        self.month31 = [1,3,5,7,8,10,12]
        self.month30 = [4,6,9,11]
        self.cleaner = Cleaner(n)
        self.replace_chars = [('_dot_dot_',':'),('_dot_', '.')] 

    #january, march, may, july, august, october, december have 31 days
    #april, june, september, november have 30 days
    #february have 28 days (for our porpuses)
    #get_day_before(int, int, int) -> list[str, str, str]
    def get_day_before(self, year, month, day):
        nday = day - 1
        nmonth = month
        nyear = year
        if nday <= 0:
            nmonth = month - 1 
            if nmonth <= 0:
                nyear = year - 1
                nmonth = 12
                nday = 31
            elif nmonth in self.month31:
                nday = 31
            elif nmonth in self.month30:
                nday = 30
            elif nyear%4 == 0:
                nday = 29
            else:
                nday = 28
        nyear = str(nyear)
        if nday < 10:
            nday = '0'+str(nday)
        else:
            nday = str(nday)
        if nmonth < 10:
            nmonth = '0'+str(nmonth)
        else:
            nmonth = str(nmonth)
        return [nyear, nmonth, nday]


    #generates a list of sentences with length lesser or equal to n
    #separating the sentences by punctuation
    def process_report(self,content, created_at, formatted_address, n=60):

        #process content
        if content != None:
            cproc = self.cleaner.remove_spanish_accents(content.encode('utf-8'))
            cproc = self.cleaner.process_abbreviated_words(cproc)
            cproc = self.cleaner.remove_links(cproc)
            cproc = self.cleaner.process_abbreviated_names(cproc)
            cproc = self.cleaner.process_common_regex(cproc)
            cproc = self.cleaner.remove_stop_characters(cproc)
            cproc = self.cleaner.remove_twitter_info(cproc)
            cproc = self.cleaner.remove_extra_spaces(cproc)
            cproc_list = self.cleaner.separate_sentences_by_punctuation(cproc)
        else:
            cproc_list = None

        #process formatted_address
        if formatted_address != None:
            faproc = self.cleaner.remove_spanish_accents(formatted_address.encode('utf-8'))
            faproc = self.cleaner.process_abbreviated_words(faproc)
            faproc = self.cleaner.process_abbreviated_names(faproc)
            faproc = self.cleaner.process_common_regex(faproc)
            faproc = self.cleaner.remove_stop_characters(faproc)
            faproc = self.cleaner.remove_twitter_info(faproc)
            faproc = self.cleaner.remove_extra_spaces(faproc)
            faproc_list = self.cleaner.separate_sentences_by_punctuation(faproc)
        else:
            faproc_list = None

        #process time
        if created_at != None:
            time_list = self.cleaner.extract_time(created_at)
        else:
            time_list = None
        
        return (cproc_list, faproc_list, time_list)


    #print word by line in a file
    def print_word_by_line(self, output_file, processed_content_tupple):
        processed_content = processed_content_tupple[0]
        processed_address = processed_content_tupple[1]
        processed_time = processed_content_tupple[2]
        
        print processed_content
        print processed_address
        print processed_time
        
        #write the processed content
        if processed_content != None:
            for sentence in processed_content:
                s = sentence.strip().split()
                #print "*** sentence = %s"%s
                if len(s) > 0:
                    nsentence = s
                    for tupple_replace in self.replace_chars:
                        aux_sentence = [n.replace(tupple_replace[0], tupple_replace[1]) for n in nsentence]
                        nsentence = aux_sentence
                    #print "*** nsentence = %s"%s
                    for word in nsentence:
                        output_file.write(word)
                    #output_file.write(word.decode('utf-8'))
                        output_file.write('\n')
                    output_file.write('\n')

        #write the processed address
        if processed_address != None:
            for sentence in processed_address:
                s = sentence.strip().split()
                if len(s) > 0:
                    nsentence = s
                    for tupple_replace in self.replace_chars:
                        aux_sentence = [n.replace(tupple_replace[0], tupple_replace[1]) for n in nsentence]
                        nsentence = aux_sentence
                    for word in nsentence:
                        output_file.write(word)
                    #output_file.write(word.decode('utf-8'))
                        output_file.write('\n')
                    output_file.write('\n')
        
        #write the processed time
        if processed_time != None:
            for word in processed_time:
                #utput_file.write(word.decode('utf-8'))
                output_file.write(word)
                output_file.write('\n')
            output_file.write('\n')
        
    
