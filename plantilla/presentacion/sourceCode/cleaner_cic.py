#!/usr/bin/python
#-*-coding: utf-8 -*-
import re

class Cleaner:
    def __init__(self, sentence_length=50):
        self.sentence_length = sentence_length
        self.accented_vowels = {'\xc3\xa1':'a', '\xc3\x81':'A',
                                '\xc3\xa9':'e', '\xc3\x89':'E',
                                '\xc3\xad':'i', '\xc3\x8d':'I',
                                '\xc3\xb3':'o', '\xc3\x93':'O',
                                '\xc3\xba':'u', '\xc3\x9a':'U',
                                '\xc3\xa0':'a', '\xc3\x80':'A', 
                                '\xc3\xa8':'e', '\xc3\x88':'E',
                                '\xc3\xac':'i', '\xc3\x8c':'I',
                                '\xc3\xb2':'o', '\xc3\x92':'O',
                                '\xc3\xb9':'u', '\xc3\x99':'U'}
        self.basic_abbreviations = ['a.', 'a.c.', 'ant.', 'aux.', 
                                    'av.', 'ave.', 'ant.', 'apo.', 
                                    'b.', 'blvd.','c.','cp.','cfe.',
                                    'c.f.e.','clin.', 'carr.','cam.', 
                                    'col.', 'const.', 'c.v.','calz.',
                                    'dr.', 'don.','e.','esq.', 'f.', 
                                    'fr.', 'frac.','fracc.','gza.', 
                                    'g.','hda.','heb.', 'h.e.b.', 'h.',
                                    'ind.', 'ing', 'i.', 'izq.','j.', 
                                    'jua.','l.', 'lic.','libr.', 'ma.',
                                    'mty.','m.', 'mtro.', 'mpal.','n.l.',
                                    'nl.','nte.','ote.', 'pte.', 'profr.',
                                    'prof.', 'prol.','priv.', 'p.', 
                                    'prot.','r.','rep.','s.a.','s.v.t.',
                                    'sta.','s.', 'sto.', 'sr.', 'sra.',
                                    'sept.', 'sec.', 'svt.', 'sc.',
                                    't.', 'trasp.','u.', 'vo.', 'univ.']
        self.stop_characters = ['|', '*', ':', '/', '(', ')', '%',
                                '[', ']', '?', '¿', '!', '¡']

        self.space_characters = ['\n', '\t','\r']

    def extract_time(self, utc_time):
        day = utc_time[:10]
        hour = utc_time[11:19]
        return [day, hour]

    def remove_spanish_accents(self, sentence):
        new_sentence = sentence.replace(',', ' , ')
        for vowel in self.accented_vowels:
            if vowel in sentence:
                aux_sentence = new_sentence.replace(vowel, self.accented_vowels[vowel])
                new_sentence = aux_sentence
        return new_sentence
                
    def process_abbreviated_words(self, sentence):
        words = sentence.strip().split()
        new_words = []
        for word in words:
            if word.lower() in self.basic_abbreviations:
                new_words.append(word.replace('.', '_dot_'))
            else:
                new_words.append(word)
        return ' '.join(new_words)

    def remove_links(self, sentence):
        words = sentence.strip().split()
        new_words = []
        for word in words:
            if 'http' in word.lower():
                continue
            new_words.append(word)
        return ' '.join(new_words)

    def process_abbreviated_names(self, sentence):
        regex_names = [r'[A-Z]\. [A-Z][a-z]+', '[A-Z]\. ?[A-Z]\. (?:del |de )?[A-Z][a-z]+']
        nsentence = sentence
        for rname in regex_names:
            found_names = re.findall(rname, nsentence)
            for name in found_names:
                nname = name.replace('.', '_dot_')
                nsentence = nsentence.replace(name, nname)
        return nsentence

    def process_common_regex(self, sentence):
        words = sentence.strip().split()
        new_words = []
        for word in words:
            composed_time = re.findall(r'(:?[A-Z]+-)?[0-9][0-9]?\.[0-9][0-9]?@[0-9][0-9]?:[0-9][0-9]?', word)
            ddtime = re.findall(r'[0-9][0-9]?:[0-9][0-9]', word)
            dtime = re.findall(r'[0-9][0-9]?\.[0-9][0-9]', word)
            two_cities = re.findall(r'[A-Za-z][a-z]+-[A-Za-z][a-z]+', word)
            if composed_time:
                word = word.replace('-', ' ')
                word = word.replace('.', '_dot_')
                word = word.replace('@', ' ')
                word = word.replace(':', '_dot_dot_')
                new_words.append(word)
            elif ddtime:
                new_words.append(word.replace(':', '_dot_dot_'))
            elif dtime:
                new_words.append(word.replace('.', '_dot_'))
            elif two_cities:
                new_words.append(word.replace('-', ' '))
            else:
                new_words.append(word)
        return ' '.join(new_words)

    def remove_stop_characters(self, sentence):
        new_sentence = sentence
        for character in self.stop_characters:
            if character in new_sentence:
                aux_sentence = new_sentence.replace(character, ' ')
                new_sentence = aux_sentence
        words = new_sentence.strip().split()
        return ' '.join(words)

    def remove_twitter_info(self, sentence):
        new_sentence = sentence
        hashtags = re.findall(r'#[a-zA-Z_0-9]+', new_sentence)
        if hashtags:
            for h in hashtags:
                aux_sentence = new_sentence.replace(h, '')
                new_sentence = aux_sentence
        via = re.findall(r'via @[a-zA-Z_0-9]+', new_sentence)
        if via:
            for v in via:
                aux_sentence = new_sentence.replace(v, '')
                new_sentence = aux_sentence
        cc = re.findall(r'cc @[a-zA-Z_]+', new_sentence)
        if cc:
            for c in cc:
                aux_sentence = new_sentence.replace(c, '')
                new_sentence = aux_sentence
        arrobas = re.findall(r'@[a-zA-Z_0-9]+', new_sentence)
        if arrobas:
            for a in arrobas:
                aux_sentence = new_sentence.replace(a, '')
                new_sentence = aux_sentence
        return new_sentence            

    def remove_extra_spaces(self, sentence):
        new_sentence = sentence
        for character in self.space_characters:
            if character in new_sentence:
                aux_sentence = new_sentence.replace(character, ' ')
                new_sentence = aux_sentence
        words = new_sentence.strip().split()
        return ' '.join(words)

    def separate_sentences_by_punctuation(self, sentence):
        sentence = sentence.replace(',', ' , ')
        words = sentence.replace('.', ' . ').strip().split()
        sentences = []
        init_sentence = 0
        end_sentence = 0
        for i in range(len(words)):
            if words[i] == '.':
                end_sentence = i + 1
                aux_sentence = ' '.join(words[init_sentence:end_sentence])
                aux_sentences = self.break_sentences_exceding_max_length(aux_sentence)
                for s in aux_sentences:
                    sentences.append(s)
                init_sentence = end_sentence
        if (end_sentence < len(words)):
            aux_sentence = ' '.join(words[init_sentence:len(words)])
            aux_sentences = self.break_sentences_exceding_max_length(aux_sentence)
            for s in aux_sentences:
                sentences.append(s)
        return sentences

    def break_sentences_exceding_max_length(self, sentence):
        words = sentence.strip().split()
        n = len(words)
        sentences = []
        if n > self.sentence_length:
            init_sentence = 0
            end_sentence = self.sentence_length
            while end_sentence <= n:
                aux_words = words[init_sentence:end_sentence]
                sentences.append(' '.join(aux_words))
                init_sentence = end_sentence
                end_sentence = init_sentence + self.sentence_length
            if (end_sentence != n):
                sentences.append(' '.join(words[init_sentence:]))
        else:
            sentences.append(sentence)
        return sentences
