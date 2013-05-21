/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*
* @file QdSeHighlighter.h
* @brief A Qt syntax highlighter for the SeExpr language
* @author  aselle
*/
#ifndef _QdSeHighlighter_h_
#define _QdSeHighlighter_h_
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QPalette>
#include <iostream>

class QdSeHighlighter : public QSyntaxHighlighter
{
    struct HighlightingRule{
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat variableFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat operatorFormat;

    int lightness;

public:
    QdSeHighlighter(QTextDocument* parent)
        :QSyntaxHighlighter(parent),lightness(130)
    {
        init();
    }

    QdSeHighlighter(QTextEdit* edit)
        :QSyntaxHighlighter(edit),lightness(130)
    {
        init();
    }

    void fixStyle(const QPalette& palette)
    {
        lightness=palette.color(QPalette::Base).value()<127 ? 250: 130;
        init();
    }

    void init()
    {
        HighlightingRule rule;
        highlightingRules.clear();

        // Operator highlighting, disabled for now
        //operatorFormat.setForeground(QColor::fromHsv(50,128,lightness));
        //QStringList operatorPatterns;
        //operatorPatterns<<"(?:->)|(?:[()\\+-/\\*%\\^:\\?\\[\\]])";
        //foreach (QString pattern,operatorPatterns){
        //    rule.pattern=QRegExp(pattern);
        //    rule.format=operatorFormat;
        //    highlightingRules.append(rule);
        //}

        numberFormat.setForeground(QColor::fromHsv(180,204,lightness));
        rule.pattern=QRegExp("\\b[0-9]*\\.[0-9]*)?|[0-9]+\\b"); // \\b?[^\\$][A-Za-z][A-Za-z0-9]*\\b");
        rule.format=numberFormat;
        //highlightingRules.append(rule);

        variableFormat.setForeground(QColor::fromHsv(200,153,lightness));
        //variableFormat.setFontWeight(QFont::Bold);
        rule.pattern=QRegExp("\\$[A-Za-z][A-Za-z0-9]*\\b");
        rule.format=variableFormat;
        highlightingRules.append(rule);

        singleLineCommentFormat.setForeground(QColor::fromHsv(210,128,lightness));
        rule.pattern=QRegExp("#[^\n]*");
        rule.format=singleLineCommentFormat;
        highlightingRules.append(rule);

    }

    void highlightBlock(const QString& text)
    {
        foreach (HighlightingRule rule,highlightingRules){
            QRegExp expression(rule.pattern);
            int index=text.indexOf(expression);
            while(index>=0){
                int length=expression.matchedLength();
                setFormat(index,length,rule.format);
                index=text.indexOf(expression,index+length);
            }
        }
        setCurrentBlockState(0);
    }
};
#endif
