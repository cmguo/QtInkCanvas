#ifndef DEPENDENCYPROPERTY_H
#define DEPENDENCYPROPERTY_H

#include <QVariant>

// namespace System.Windows

class DependencyObject;
class DependencyPropertyChangedEventArgs;

class DefaultValueFactory
{
public:
    virtual ~DefaultValueFactory() {}
    virtual QVariant DefaultValue() = 0;
};

Q_DECLARE_METATYPE(DefaultValueFactory*)

class DependencyProperty
{
public:
    typedef std::function<void(DependencyObject&, DependencyPropertyChangedEventArgs&)> PropertyChangedCallback;

    typedef std::function<bool(QVariant)> ValidateValueCallback;

    DependencyProperty(DefaultValueFactory* dflt)
        : DependencyProperty(QVariant::fromValue(dflt))
    {
    }

    DependencyProperty(DefaultValueFactory* dflt, PropertyChangedCallback changed)
        : DependencyProperty(QVariant::fromValue(dflt), changed)
    {
    }

    DependencyProperty(DefaultValueFactory* dflt, PropertyChangedCallback changed, ValidateValueCallback validate)
        : DependencyProperty(QVariant::fromValue(dflt), changed, validate)
    {
    }

    DependencyProperty(QVariant dflt);

    DependencyProperty(QVariant dflt, PropertyChangedCallback changed);

    DependencyProperty(QVariant dflt, PropertyChangedCallback changed, ValidateValueCallback validate);

    static QVariant const UnsetValue;

    int GlobalIndex() const;

    QVariant DefaultValue() const;

    void Changed(DependencyObject& d, QVariant old, QVariant new_) const;

private:
    QVariant default_;
    std::function<void(DependencyObject&, DependencyPropertyChangedEventArgs&)> changed_;
    std::function<bool(QVariant)> validate_;
};

#endif // DEPENDENCYPROPERTY_H
